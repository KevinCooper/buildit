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
#include "hash.h"
#include "argv.h"
#include "functions.h"

void printMD5(char * toPrint);

int main(int argc, char * argv[]) {
	allMahHashes = ht_create(65536);
	/* "opaque" encryption, decryption ctx structures that libcrypto uses to record
	 status of enc/dec operations */
	EVP_CIPHER_CTX en, de;
	unsigned int salt[] = { 12345, 54321 };
	unsigned char *key_data;
	int key_data_len;
	char interString[256];

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

	if (args.returnStatus == -1)
		invalid();

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
		newFile = 1;
		mahFile = fopen(args.logName, "w+");
	} else {
		mahFile = fopen(args.logName, "r+");
	}
	if (mahFile == NULL)
		invalid();

	//Compute the MD5 of the file
	MD5_Init(&mdContext);
	MD5_Init(&mdContextWithUserInput);
	if (!newFile) {
		while ((read = getline(&line, &bytes, mahFile)) != -1 && fileSize > 16) {

			int len = strlen(line);
			fileSize = fileSize - len;

			// RERUN COMMANDS CAUZE LOGIC!
			sprintf(interString, "./logappend %s", line);
			int tempc;
			char ** tempv = argv_split(interString, &tempc);
			logappend_args temp = opt_parser(tempc, tempv);
			bzero(interString, 256);
			argv_free(tempv);
			if (check_logic(&temp) == -1)
				return -1;
			// FINISH LOGICZ

			MD5_Update(&mdContext, line, len);
			MD5_Update(&mdContextWithUserInput, line, len);
		}
	}
	if (check_logic(&args) == -1)
		return -1;
	MD5_Update(&mdContextWithUserInput, args.toString, strlen(args.toString));
	MD5_Final(currentFileMD5, &mdContext);
	MD5_Final(fileWithInputMD5, &mdContextWithUserInput);

	//Mah debug printf's
	printf("%s", "MD5 of the current file:    ");
	printMD5(currentFileMD5);
	printf("%s", "MD5 of the updated file:    ");
	printMD5(fileWithInputMD5);

	//Check the old md5
	int status;
	if (!newFile) {
		//Encrypted MD5 should be last 16 bytes of the file
		status = fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
		status = fread(encryptedFileMD5, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);
		printf("The encrypted MD5 was:      ");
		printMD5(encryptedFileMD5);
		decryptedFileMD5 = (char *) aes_decrypt(&de, encryptedFileMD5, len);
		decryptedFileMD5[MD5_DIGEST_LENGTH] = '\0';
		printf("%s", "Decrypted current file MD5: ");
		printMD5(decryptedFileMD5);

		/* DISABLE CRYPTO CHECK UNTIL WORKING
		 for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		 if ((unsigned char)decryptedFileMD5[i] != (unsigned char)currentFileMD5[i]) {
		 fclose(mahFile);
		 return -1;
		 }
		 }
		 */
		//Go back to get ready to write over the old key with our new line of input
		status = fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
	}
	status = fwrite(args.toString, 1, strlen(args.toString), mahFile);

	//Write encrypted MD5
	encryptedFileWithInputMD5 = aes_encrypt(&en, fileWithInputMD5, len);
	status = fwrite(encryptedFileWithInputMD5, 1, MD5_DIGEST_LENGTH, mahFile);
	printf("Wrote the encrypted MD5:    ");
	printMD5(encryptedFileWithInputMD5);



	//Cleanup
	fclose(mahFile);
	fflush(mahFile);
	EVP_CIPHER_CTX_cleanup(&en);
	EVP_CIPHER_CTX_cleanup(&de);
	return 0;

}

void printMD5(char * toPrint) {
	int i;
	printf("0x");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", (unsigned char)toPrint[i]);
	printf("%s", "\n");
}


