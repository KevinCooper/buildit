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

void batch(logappend_args args);
void processLine(logappend_args args, int32_t CheckAndHashBool);
void checkMahFile(logappend_args args);
void inter(logappend_args args);

#define NULL_CHECK(val)  if (val == NULL) invalid();

MD5_CTX oldMD5;
MD5_CTX currentMD5;
MD5_CTX newMD5;

unsigned char oldMD5String[MD5_DIGEST_LENGTH + 1];
unsigned char currentMD5String[MD5_DIGEST_LENGTH + 1];
unsigned char newMD5String[MD5_DIGEST_LENGTH + 1];

uint32_t oldHashExists;
uint32_t firstRun;

HT* allMahHashes_guests = NULL;
HT* allMahHashes_employees = NULL;

int main(int argc, char * argv[]) {
	if (argc < 3)
		invalid();
	oldHashExists = 1;
	oldTime = -1;
	firstRun = 1;
	logappend_args args = opt_parser(argc, argv, 1);
	allMahHashes_employees = ht_create(65536);
	allMahHashes_guests = ht_create(65536);

	if (args.returnStatus == -1)
		invalid();

	if (args.batchFile) {
		isBatch = 1;
		batch(args);
	} else {
		isBatch = 0;
		checkMahFile(args);
		inter(args);
		if (check_logic(&args) == -1)
			invalid();
		processLine(args, 1);
	}
	if (isBatch) {
		return 0;
	} else {
		return 0;
	}
}

void inter(logappend_args args) {
	int32_t fileSize = 0;
	FILE* mahFile = NULL;
	size_t bytes = 0;
	ssize_t read = 0;
	char * line = NULL;
	char interString[256];

	fileSize = fsize(args.logName);
	if (fileSize < 16) {
		return;
	} else {
		mahFile = fopen(args.logName, "r");
	}

	while ((read = getline(&line, &bytes, mahFile)) != -1 && fileSize > 16) {

		int len = strlen(line);
		fileSize = fileSize - len;
		bzero(interString, 256);
		// RERUN COMMANDS CAUZE LOGIC!
		sprintf(interString, "./logappend %s", line);
		int tempc;
		char ** tempv = argv_split(interString, &tempc);
		logappend_args temp = opt_parser(tempc, tempv, 0);
		if (check_logic(&temp) == -1)
			continue;
		argv_free(tempv);
		// FINISH LOGICZ
	}
	fclose(mahFile);
}

void batch(logappend_args args) {
	FILE* batchFile = NULL;
	int32_t fileSize = 0;
	size_t bytes = 0;
	ssize_t read = 0;
	char * line = NULL;
	char interString[256];

	fileSize = fsize(args.batchFile);
	if (fileSize < 10)
		invalid();
	batchFile = fopen(args.batchFile, "r+");

	while ((read = getline(&line, &bytes, batchFile)) != -1 && fileSize > 16) {

		int len = strlen(line);
		fileSize = fileSize - len;
		// RERUN COMMANDS CAUZE LOGIC!
		sprintf(interString, "./logappend %s", line);
		int tempc;
		char ** tempv = argv_split(interString, &tempc);
		logappend_args temp = opt_parser(tempc, tempv, 1);
		if (temp.batchFile)
			continue;
		if (firstRun) {
			checkMahFile(temp);
			inter(temp);
		}
		if (check_logic(&temp) == -1)
			continue;
		processLine(temp, fileSize < 10 ? 1 : 0);
		firstRun = 0;
		bzero(interString, 256);
		argv_free(tempv);
		// FINISH LOGICZ
	}

}

void processLine(logappend_args args, int32_t isLastLine) {
	unsigned char newMD5_S[MD5_DIGEST_LENGTH + 1];
	MD5_Update(&newMD5, args.toString, strlen(args.toString));
	int32_t fileSize = 0;
	FILE* mahFile = NULL;
	unsigned int md5len = MD5_DIGEST_LENGTH;
	//Make sure it is a good new first line

	fileSize = fsize(args.logName);
	if (fileSize < 16) {
		mahFile = fopen(args.logName, "w+");
	} else {
		mahFile = fopen(args.logName, "r+");
	}
	NULL_CHECK(mahFile)
	//Write the current line to the file
	if (fileSize < 16) {
		fwrite(args.toString, sizeof(char), strlen(args.toString), mahFile);
	} else {
		if (firstRun) {
			fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
			firstRun = 0;
		} else {
			fseek(mahFile, 0, SEEK_END);
		}
		fwrite(args.toString, sizeof(char), strlen(args.toString), mahFile);
	}
	if (isLastLine) {
		MD5_Final(newMD5_S, &newMD5);
		EVP_CIPHER_CTX en, de;
		unsigned int salt[] = { 12345, 54321 };
		if (aes_init(args.token, strlen(args.token), (unsigned char *) &salt,
				&en, &de)) {
			printf("Couldn't initialize AES cipher\n");
			invalid();
		}
		char * ciphertext = aes_encrypt(&en, newMD5_S, &md5len);
		fwrite(ciphertext, sizeof(char), MD5_DIGEST_LENGTH, mahFile);
	}
	fclose(mahFile);
}

void checkMahFile(logappend_args args) {
	int32_t fileSize = 0;
	FILE* mahFile = NULL;
	char * pathname = args.logName;

	unsigned int md5len = MD5_DIGEST_LENGTH;
	char* oldMD5 = calloc(MD5_DIGEST_LENGTH + 1, 1);
	MD5_Init(&currentMD5);
	MD5_Init(&newMD5);
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
		MD5_Update(&newMD5, line, len);
	}
	unsigned char currentMD5_S[MD5_DIGEST_LENGTH + 1];
	MD5_Final(currentMD5_S, &currentMD5);

	//Read encrypted MD5 from the file
	fseek(mahFile, -1 * MD5_DIGEST_LENGTH, SEEK_END);
	fread(oldMD5, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);

	//Decrypt the old MD5
	EVP_CIPHER_CTX en, de;
	unsigned int salt[] = { 12345, 54321 };
	if (aes_init(args.token, strlen(args.token), (unsigned char *) &salt, &en,
			&de)) {
		printf("Couldn't initialize AES cipher\n");
		invalid();
	}
	char* oldMD5_de_S = (char *) aes_decrypt(&de, oldMD5, &md5len);

	if (memcmp(oldMD5_de_S, currentMD5_S, MD5_DIGEST_LENGTH)) {
		invalid_token();
	}

	fclose(mahFile);
	return;
}

