#include "dbg.h"
#include "args.h"
#include "definitions.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <openssl/md5.h>
#include <stdlib.h>

off_t fsize(const char *filename);

int main(int argc, char * argv[]) {
	int32_t newFile = 0;
	logappend_args args = opt_parser(argc, argv);
	char * line = NULL;
	size_t bytes = 0;
	ssize_t read;
	MD5_CTX mdContext;
	MD5_CTX mdContextFinal;
	int i = 0;
	int32_t fileSize;
	FILE* mahFile;
	unsigned char md5String[MD5_DIGEST_LENGTH];
	unsigned char md5StringFile[MD5_DIGEST_LENGTH];
	unsigned char md5StringFinal[MD5_DIGEST_LENGTH];

	if (args.returnStatus == -1) {
		printf("invalid\n");
		return -1;
	}

	if (args.batchFile) {
		//TODO: Batch interpreter call.  Not implemented yet.
		return 1;
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
	MD5_Init(&mdContextFinal);
	if (!newFile) {
		while ((read = getline(&line, &bytes, mahFile)) != -1 && fileSize>16) {
			int len = strlen(line);
			fileSize = fileSize - len;
			MD5_Update(&mdContext, line, len);
			MD5_Update(&mdContextFinal, line, len);
		}
	}
	MD5_Update(&mdContextFinal, args.toString, strlen(args.toString));
	MD5_Final(md5String, &mdContext);
	MD5_Final(md5StringFinal, &mdContextFinal);
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", md5String[i]);
	printf("%s","\n");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
		printf("%02x", md5StringFinal[i]);
	fflush(stdout);
	//Check the old md5
	int status;
	if (!newFile) {
		status = fseek(mahFile, -1*MD5_DIGEST_LENGTH, SEEK_END);
		status = fread(md5StringFile, sizeof(unsigned char), MD5_DIGEST_LENGTH, mahFile);
		for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
			if (md5StringFile[i] != md5String[i]) {
				fclose(mahFile);
				return -1;
			}
		}
		status = fseek(mahFile, -1*MD5_DIGEST_LENGTH, SEEK_END);
	}
	status = fwrite(args.toString, 1, strlen(args.toString), mahFile);
	debug("Wrote this many bytes: %d", status);
	status = fwrite(md5StringFinal, 1, MD5_DIGEST_LENGTH, mahFile);
	debug("Wrote this many bytes: %d", status);
	fclose(mahFile);
	fflush(mahFile);
	debug("The string of this is: %s\nIt should be in : %s", args.toString, args.logName);
	return 0;

}

off_t fsize(const char *filename) {
	struct stat st;

	if (stat(filename, &st) == 0)
		return st.st_size;

	return -1;
}

