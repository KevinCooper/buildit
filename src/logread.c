#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include "args.h"
#include "definitions.h"
#include "functions.h"
#include "args_log.h"
#include "argv.h"

void checkMahFile(logread_args args);

int main(int argc, char * argv[]) {
	int32_t fileSize = 0;
	FILE* file = NULL;
	size_t bytes = 0;
	ssize_t read = 0;
	char * line = NULL;
	char interString[256];

	//get Logreader arguements
	logread_args args = opt_parser(argc, argv);
	//Verify integrity and hopefully the syntax should be right
	checkMahFile(args);

	fileSize = fsize(args.logName);
	if (fileSize < 10)
		invalid();
	file = fopen(args.logName, "r");
	//Line by line apply the options
	while ((read = getline(&line, &bytes, file)) != -1 && fileSize > 16) {

		int len = strlen(line);
		fileSize = fileSize - len;
		// RERUN COMMANDS CAUZE LOGIC!
		sprintf(interString, "./logappend %s", line);
		int tempc;
		char ** tempv = argv_split(interString, &tempc);
		logappend_args temp = opt_parser_log(tempc, tempv);
		bzero(interString, 256);
		argv_free(tempv);
		// FINISH LOGICZ
	}
	return 0;
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
	int status = 0;

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
	status = fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
	status = fread(oldMD5, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);

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
