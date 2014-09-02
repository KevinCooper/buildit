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

int main(int argc, char * argv[]) {
	int32_t fileSize=0;
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
}

