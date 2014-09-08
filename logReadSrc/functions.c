#include "functions.h"

void nameOpt(char * input) {
	reti = regcomp(&regex, "[^a-zA-Z0-9]", 0);
	reti = regexec(&regex, input, 0, NULL, 0);
	if (!reti) {
		invalid();
	}
	regfree(&regex);
}

void numOpt(char * input) {
	reti = regcomp(&regex, "[^0-9]", 0);
	reti = regexec(&regex, input, 0, NULL, 0);
	if (!reti) {
		invalid();
	}
	regfree(&regex);
}

void printMD5(char * toPrint) {
	int i;
	printf("0x");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", (unsigned char) toPrint[i]);
	printf("%s", "\n");
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

void invalid() {
	printf("invalid\n");
	exit(-1);
}

void invalid_0(){
	printf("invalid\n");
	exit(0);
}
void checkMahFile(logread_args args) {
	int32_t fileSize = 0;
	FILE* mahFile = NULL;
	char * pathname = args.logName;
	MD5_CTX currentMD5;

	unsigned int md5len = MD5_DIGEST_LENGTH;
	char* oldMD5 = calloc(MD5_DIGEST_LENGTH + 1, 1);
	MD5_Init(&currentMD5);
	char * line = NULL;
	size_t bytes = 0;
	ssize_t read;

	fileSize = fsize(pathname);
	if (fileSize > 16) {
		mahFile = fopen(pathname, "r+");
	} else {
		return;
	}
	//Create an MD5 of the file and verify the syntax stuff
	while ((read = getline(&line, &bytes, mahFile)) != -1 && fileSize > 16) {
		int len = strlen(line);
		fileSize = fileSize - len;
		MD5_Update(&currentMD5, line, len);
	}
	char currentMD5_S[MD5_DIGEST_LENGTH + 1];
	MD5_Final(currentMD5_S, &currentMD5);

	//Read encrypted MD5 from the file
	fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
	fread(oldMD5, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);

	//Decrypt the old MD5
	EVP_CIPHER_CTX en, de;
	unsigned int salt[] = { 12345, 54321 };
	if (aes_init(args.token, strlen(args.token), (unsigned char *) &salt, &en, &de)) {
		printf("Couldn't initialize AES cipher\n");
		invalid();
	}
	char* oldMD5_de_S = (char *) aes_decrypt(&de, oldMD5, &md5len);

	if (memcmp(oldMD5_de_S, currentMD5_S, MD5_DIGEST_LENGTH)) {
		invalid();
	}

	fclose(mahFile);
	return;
}


