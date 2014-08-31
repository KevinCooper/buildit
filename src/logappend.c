#include "dbg.h"
#include "args.h"
#include "definitions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

off_t fsize(const char *filename);
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx);
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len);
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len);

int main(int argc, char * argv[]) {

	/* "opaque" encryption, decryption ctx structures that libcrypto uses to record
	 status of enc/dec operations */
	EVP_CIPHER_CTX en, de;
	unsigned int salt[] = { 12345, 54321 };
	unsigned char *key_data;
	int key_data_len;

	int32_t newFile = 0;
	logappend_args args = opt_parser(argc, argv);
	char * line = NULL;
	size_t bytes = 0;
	ssize_t read;
	MD5_CTX mdContext;
	MD5_CTX mdContextWithUserInput;
	int i = 0;
	int32_t fileSize;
	FILE* mahFile;
	int NINJA = MD5_DIGEST_LENGTH;
	int * len = &NINJA;

	unsigned char currentFileMD5[MD5_DIGEST_LENGTH + 1];
	unsigned char encryptedFileMD5[MD5_DIGEST_LENGTH + 1];
	unsigned char fileWithInputMD5[MD5_DIGEST_LENGTH + 1];
	char * encryptedFileWithInputMD5 = calloc(MD5_DIGEST_LENGTH + 1, 0);
	char * decryptedFileMD5 = calloc(MD5_DIGEST_LENGTH + 1, 0);

	if (args.returnStatus == -1) {
		printf("invalid\n");
		return -1;
	}

	if (args.batchFile) {
		//TODO: Batch interpreter call.  Not implemented yet.
		return 1;
	}

	if (aes_init(args.token, strlen(args.token), (unsigned char *) &salt, &en, &de)) {
		printf("Couldn't initialize AES cipher\n");
		return -1;
	}

	//Open up the file or create it if it doesn't exist
	fileSize = fsize(args.logName);
	if (fileSize < 16) {
		debug("%s", "File is too short to have an MD5, must be a new file");
		newFile = 1;
		mahFile = fopen(args.logName, "w+");
	} else {
		mahFile = fopen(args.logName, "r+");
	}
	if (mahFile == NULL)
		return -1;

	//Compute the MD5 of the file
	MD5_Init(&mdContext);
	MD5_Init(&mdContextWithUserInput);
	if (!newFile) {
		while ((read = getline(&line, &bytes, mahFile)) != -1 && fileSize > 16) {
			int len = strlen(line);
			fileSize = fileSize - len;
			MD5_Update(&mdContext, line, len);
			MD5_Update(&mdContextWithUserInput, line, len);
		}
	}
	MD5_Update(&mdContextWithUserInput, args.toString, strlen(args.toString));
	MD5_Final(currentFileMD5, &mdContext);
	MD5_Final(fileWithInputMD5, &mdContextWithUserInput);
	printf("%s", "MD5 of the current file:");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", currentFileMD5[i]);
	printf("%s", "\n");
	printf("%s", "MD5 of the current file with input:");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", fileWithInputMD5[i]);
	printf("%s", "\n");
	fflush(stdout);
	//Check the old md5
	int status;
	if (!newFile) {
		status = fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
		status = fread(encryptedFileMD5, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);
		encryptedFileMD5[MD5_DIGEST_LENGTH] = '\0';
		printf("The encrypted md5 was: %s\n", encryptedFileMD5);
		decryptedFileMD5 = (char *) aes_decrypt(&de, encryptedFileMD5, len);
		decryptedFileMD5[MD5_DIGEST_LENGTH] = '\0';
		printf("%s", "Decrypted MD5 of the current file:");
		for (i = 0; i < MD5_DIGEST_LENGTH; i++)
			printf("%02x", decryptedFileMD5[i]);
		printf("%s", "\n");
		for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
			if ((unsigned char)decryptedFileMD5[i] != (unsigned char)currentFileMD5[i]) {
				fclose(mahFile);
				return -1;
			}
		}
		status = fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
	}
	status = fwrite(args.toString, 1, strlen(args.toString), mahFile);

	//Write encrypted MD5
	encryptedFileWithInputMD5 = aes_encrypt(&en, fileWithInputMD5, len);
	status = fwrite(encryptedFileWithInputMD5, 1, MD5_DIGEST_LENGTH, mahFile);
	debug("Wrote the encrypted MD5: %s", encryptedFileWithInputMD5);
	fclose(mahFile);
	fflush(mahFile);
	EVP_CIPHER_CTX_cleanup(&en);
	EVP_CIPHER_CTX_cleanup(&de);
	return 0;

}

off_t fsize(const char *filename) {
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;

	return -1;
}

/**
 * Create an 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char *key_data, int key_data_len, unsigned char *salt, EVP_CIPHER_CTX *e_ctx, EVP_CIPHER_CTX *d_ctx) {
	int i, nrounds = 5;
	unsigned char key[32], iv[32];

	/*
	 * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
	 * nrounds is the number of times the we hash the material. More rounds are more secure but
	 * slower.
	 */
	i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
	if (i != 32) {
		printf("Key size is %d bits - should be 256 bits\n", i);
		return -1;
	}

	EVP_CIPHER_CTX_init(e_ctx);
	EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
	EVP_CIPHER_CTX_init(d_ctx);
	EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

	return 0;
}

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char *aes_encrypt(EVP_CIPHER_CTX *e, unsigned char *plaintext, int *len) {
	/* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
	int c_len = *len + AES_BLOCK_SIZE;
	int f_len = 0;
	unsigned char *ciphertext = malloc(c_len);

	/* allows reusing of 'e' for multiple encryption cycles */
	EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

	/* update ciphertext, c_len is filled with the length of ciphertext generated,
	 *len is the size of plaintext in bytes */
	EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

	/* update ciphertext with the final remaining bytes */
	EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

	*len = c_len + f_len;
	return ciphertext;
}

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char *aes_decrypt(EVP_CIPHER_CTX *e, unsigned char *ciphertext, int *len) {
	/* because we have padding ON, we must allocate an extra cipher block size of memory */
	int p_len = *len, f_len = 0;
	unsigned char *plaintext = malloc(p_len + AES_BLOCK_SIZE);

	EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
	EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
	EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

	*len = p_len + f_len;
	return plaintext;
}
