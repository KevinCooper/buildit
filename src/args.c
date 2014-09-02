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

logread_args opt_parser(int32_t argc, char **argv) {
	int32_t index;
	int32_t c;
	logread_args args;
	args.token = NULL;
	args.employeeName = NULL;
	args.guestName = NULL;
	args.logName = NULL;
	args.inHTML = 0;
	args.toSTDOUT = 0;
	args.totalTime = 0;
	args.listAllRooms_R = 0;
	args.printSpecificRooms_I = 0;
	args.listEmployeesWithTime = 0;
	args.listEmployeesWithoutTime = 0;
	args.lowerBound = -1;
	args.upperBound = -1;

	opterr = 0;
	int32_t len = 0;
	optind = 0;  //This must occur to have getopt back in its correct state!

//TODO: Check for invalid inputs (Token Flag with no opt, will give ":" )

	while ((c = getopt(argc, argv, "K:HSRTAIL:U:E:G:")) != -1) {
		switch (c) {
		case 'H':
			args.inHTML = 1;
			break;
		case 'S':
			args.toSTDOUT = 1;
			break;
		case 'R':
			args.listAllRooms_R = 1;
			break;
		case 'T':
			args.totalTime = 1;
			break;
		case 'I':
			args.printSpecificRooms_I = 1;
			break;
		case 'A':
			args.listEmployeesWithTime = 1;
			break;
		case 'B':
			args.listEmployeesWithoutTime = 1;
			break;
		case 'K':
			//TODO: Verify that token is alphanumeric
			len = strlen(optarg);
			args.token = (char *) malloc(len + 1);
			strcpy(args.token, optarg);
			break;
		case 'E':
			//TODO: verify that name is upper/lowercase letters.  No spaces.
			len = strlen(optarg);
			args.employeeName = (char *) malloc(len + 1);
			strcpy(args.employeeName, optarg);
			break;
		case 'G':
			len = strlen(optarg);
			args.guestName = (char *) malloc(len + 1);
			strcpy(args.guestName, optarg);
			break;
		case 'L':
			args.lowerBound = atoi(optarg);
			break;
		case 'U':
			args.upperBound = atoi(optarg);
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
			exit(-1);
		}
	}

	index = optind;
	if (index < argc) {
		args.logName = argv[index];
	}
	int32_t exclusive_options = args.toSTDOUT + args.listAllRooms_R + args.totalTime + args.printSpecificRooms_I + args.listEmployeesWithTime
			+ args.listEmployeesWithoutTime;
	if (exclusive_options > 1 || exclusive_options < 1)
		exit(-1);

	return args;
}

