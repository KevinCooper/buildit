#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include "dbg.h"
#include "args.h"

#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) ){
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1)

logappend_args opt_parser(int32_t argc, char **argv) {
	int32_t index;
	int32_t c;
	logappend_args args;
	args.token = NULL;
	args.employeeName = NULL;
	args.guestName = NULL;
	args.logName = NULL;
	args.batchFile = NULL;
	args.eventArrival = -1;
	args.eventDeparture = -1;
	args.roomID = -1;
	args.timestamp = -1;
	args.toString = NULL;
	opterr = 0;
	int32_t len = 0;
	optind = 0;  //This must occur to have getopt back in its correct state!

//TODO: Check for invalid inputs (Token Flag with no opt, will give ":" )

	while ((c = getopt(argc, argv, "T:B:K:E:G:ALR:")) != -1) {
//		debug("%d", c);
		switch (c) {
		case 'T':
			args.timestamp = atoi(optarg);
			break;
		case 'B':
			len = strlen(optarg);
			args.batchFile = (char *) malloc(len + 1);
			args.batchFile = strdup(optarg);
			*(args.batchFile + len) = '\0';
			break;
		case 'K':
			//TODO: Verify that token is alphanumeric
			len = strlen(optarg);
			args.token = (char *) malloc(len + 1);
			args.token = strdup(optarg);
			*(args.token + len) = '\0';
			break;
		case 'E':
			//TODO: verify that name is upper/lowercase letters.  No spaces.
			len = strlen(optarg);
			args.employeeName = (char *) malloc(len + 1);
			args.employeeName = strdup(optarg);
			*(args.employeeName + len) = '\0';
			break;
		case 'G':
			len = strlen(optarg);
			args.guestName = (char *) malloc(len + 1);
			args.guestName = strdup(optarg);
			*(args.guestName + len) = '\0';
			break;
		case 'A':
			args.eventArrival = 1;
			break;
		case 'L':
			args.eventDeparture = 1;
			break;
		case 'R':
			args.roomID = atoi(optarg);
			break;
		case '?':
			if (optopt == 'c')
				debug("Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				debug("Unknown option `-%c'.\n", optopt);
			else
				debug("Unknown option character `\\x%x'.\n", optopt);
			break;
		default:
			goto BAD;
		}
	}

	index = optind;
	if (index < argc) {
		args.logName = argv[index];
	}
	//debug(
	//		"\nTimestamp: %d\nToken: %s\nEmployee: %s\nGuest: %s\nArrival: %d\nDeparture: %d\nRoomID: %d\nLogName: %s\nBatchName: %s",
	//		args.timestamp, args.token, args.employeeName, args.guestName,
	//		args.eventArrival, args.eventDeparture, args.roomID, args.logName,
	//		args.batchFile);

	if (args.batchFile != NULL) {
		args.returnStatus = 1;
	} else if (args.timestamp
			== -1|| args.token == NULL || args.logName == NULL) {
		args.returnStatus = -1;
	} else if ((args.eventArrival == -1 && args.eventDeparture == -1)
			|| (args.eventArrival == 1 && args.eventDeparture == 1)) {
		args.returnStatus = -1;
	} else if ((args.employeeName == NULL && args.guestName == NULL)
			|| (args.employeeName != NULL && args.guestName != NULL)) {
		args.returnStatus = -1;
	} else if (args.roomID == -1
			&& !(args.eventDeparture == 1 || args.eventArrival == 1)) {
		args.returnStatus = -1;
	} else {
		args.returnStatus = 0;
	}
	if (args.returnStatus != -1)
		toString(&args);
	return args;

	BAD: ;
	args.returnStatus = -1;
	return args;
}

//FIXME: Maybe strings larger than 256?
void * toString(logappend_args* args) {
	char * string = (char *) calloc(256, 1);
	char str[15];
	if (args->batchFile != NULL) {
		sprintf(string, "-B %s ", args->batchFile);
	} else {
		sprintf(string, "-T %d ", args->timestamp);
		if (args->eventArrival == -1) {
			strcat(string, "-L ");
		} else {
			strcat(string, "-A ");
		}
		if (args->employeeName == NULL) {
			strcat(string, "-E ");
			strcat(string, args->guestName);
		} else {
			strcat(string, "-G ");
			strcat(string, args->employeeName);
		}
		if (args->roomID != -1) {
			strcat(string, " -R ");
			sprintf(str, "%d", args->roomID);
			strcat(string, str);
			strcat(string, " ");
			strcat(string, args->logName);
		} else {
			strcat(string, " ");
			strcat(string, args->logName);
		}
	}
	strcat(string, "\n");
	args->toString = string;
	return 0;
}
