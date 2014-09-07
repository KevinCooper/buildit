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
	args.currentState = 0;
	args.totalTime = 0;
	args.listAllRooms_R = 0;
	args.printSpecificRooms_I = 0;
	args.listEmployeesWithTime = 0;
	args.listEmployeesWithoutTime = 0;
	args.bounds = malloc(sizeof(times));
	args.bounds->lower = 0;
	args.bounds->upper = 0;
	args.bounds->lower1 = 0;
	args.bounds->upper1 = 0;
	args.peoples_I = NULL;
	opterr = 0;
	int32_t len = 0;
	optind = 0;  //This must occur to have getopt back in its correct state!

//TODO: Check for invalid inputs (Token Flag with no opt, will give ":" )

	while ((c = getopt(argc, argv, "K:BHSRTAIL:U:E:G:")) != -1) {
		switch (c) {
		case 'H':
			args.inHTML = 1;
			break;
		case 'S':
			args.currentState = 1;
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
			len = strlen(optarg);
			args.token = (char *) malloc(len + 1);
			strcpy(args.token, optarg);
			break;
		case 'E':
			len = strlen(optarg);
			if (args.printSpecificRooms_I) {
				char * person = (char *) malloc(len + 1);
				strcpy(person, optarg);
				stack_push(&args.peoples_I, person);
			} else {
				args.employeeName = (char *) malloc(len + 1);
				strcpy(args.employeeName, optarg);
			}
			break;
		case 'G':
			len = strlen(optarg);
			if (args.printSpecificRooms_I) {
				char * person = (char *) malloc(len + 1);
				strcpy(person, optarg);
				stack_push(&args.peoples_I, person);
			} else {
				args.guestName = (char *) malloc(len + 1);
				strcpy(args.guestName, optarg);
			}
			break;
		case 'L':
			if (args.listEmployeesWithoutTime
					&& args.bounds->upper > args.bounds->lower) {
				args.bounds->lower1 = atoi(optarg);
			} else {
				args.bounds->lower = atoi(optarg);
			}
			break;
		case 'U':
			if (args.listEmployeesWithoutTime
					&& args.bounds->upper > args.bounds->lower) {
				args.bounds->upper1 = atoi(optarg);
			} else {
				args.bounds->upper = atoi(optarg);
			}
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
			invalid();
		}
	}

	index = optind;
	if (index < argc) {
		args.logName = argv[index];
	}
	int32_t exclusive_options = args.currentState + args.listAllRooms_R
			+ args.totalTime + args.printSpecificRooms_I
			+ args.listEmployeesWithTime + args.listEmployeesWithoutTime;
	if (exclusive_options > 1 || exclusive_options < 1)
		invalid();

	return args;
}

