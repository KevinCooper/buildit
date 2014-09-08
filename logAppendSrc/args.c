#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <unistd.h>
#include "dbg.h"
#include "args.h"
#include "functions.h"
#include "definitions.h"

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
		//debug("%c", c);
		switch (c) {
		case 'T':
			numOpt(optarg);
			args.timestamp = atoi(optarg);
			break;
			len = MIN(strlen(optarg), MAX_ONE);
			args.batchFile = (char *) calloc(MAX, 1);
			nameOpt(optarg);
			strncpy(args.batchFile, optarg, len);
			break;
		case 'K':
			len = MIN(strlen(optarg), MAX_ONE);
			args.token = (char *) calloc(MAX, 1);
			nameOpt(optarg);
			strncpy(args.token, optarg, len);
			break;
		case 'E':
			len = MIN(strlen(optarg), MAX_ONE);
			args.employeeName = (char *) calloc(MAX, 1);
			nameOpt(optarg);
			strncpy(args.employeeName, optarg, len);
			break;
		case 'G':
			len = MIN(strlen(optarg), MAX_ONE);
			args.guestName = (char *) calloc(MAX, 1);
			nameOpt(optarg);
			strncpy(args.guestName, optarg, len);
			break;
		case 'A':
			args.eventArrival = 1;
			break;
		case 'L':
			args.eventDeparture = 1;
			break;
		case 'R':
			numOpt(optarg);
			args.roomID = atoi(optarg);
			break;
		case '?':
			invalid();
			break;
		default:
			invalid();
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

	} else if (args.timestamp
			== -1|| args.token == NULL || args.logName == NULL) {

	} else if ((args.eventArrival == -1 && args.eventDeparture == -1)
			|| (args.eventArrival == 1 && args.eventDeparture == 1)) {
		invalid();
	} else if ((args.employeeName == NULL && args.guestName == NULL)
			|| (args.employeeName != NULL && args.guestName != NULL)) {
		invalid();
	} else if (args.roomID == -1
			&& !(args.eventDeparture == 1 || args.eventArrival == 1)) {
		invalid();
	} else {
		args.returnStatus = 0;
	}
	toString(&args);
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
			strcat(string, "-G ");
			strcat(string, args->guestName);
		} else {
			strcat(string, "-E ");
			strcat(string, args->employeeName);
		}
		if (args->roomID != -1) {
			strcat(string, " -R ");
			sprintf(str, "%d", args->roomID);
			strcat(string, str);
		}
	}
	strcat(string, "\n");
	args->toString = string;
	return 0;
}
