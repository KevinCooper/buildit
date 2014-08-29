#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <unistd.h>
#include "dbg.h"
#include "args.h"

//int main( int argc, char **argv[])
logappend_args opt_parser(int32_t argc, char **argv, error_t*  status)
{
	//error_t*  status;
	int i = 0;
	for (i =0; i< argc; i++){
		debug("%s",argv[i]);
	}
	int32_t index;
	int32_t c;
	logappend_args args;
	args.token = NULL; args.employeeName = NULL; args.guestName = NULL; args.logName = NULL; args.batchFile = NULL; args.eventArrival =0; args.eventDeparture=0;args.roomID=0;
	opterr = 0;
	uint32_t len = 0;
//TODO: Check for invalid inputs (Token Flag with no opt, will give ":" )
  while ((c = getopt (argc, argv, ":T:B:K:E:G:ALR:")) != -1)
    switch (c)
      {
      case 'T':
        args.timestamp = atoi(optarg);
        break;
      case 'B':
    	len = strlen(optarg);
        args.batchFile = (char *) malloc(len + 1);
        args.batchFile = strdup(optarg);
        *(args.batchFile + len)='\0';
        break;
      case 'K':
    	//TODO: Verify that token is alphanumeric
      	len = strlen(optarg);
        args.token = (char *) malloc(len + 1);
        args.token = strdup(optarg);
        *(args.token + len)='\0';
        break;
      case 'E':
    	//TODO: verify that name is upper/lowercase letters.  No spaces.
    	if (args.guestName != NULL ){
    		goto INVALID;
    	}
    	uint32_t len = strlen(optarg);
        args.employeeName = (char *) malloc(len + 1);
        args.employeeName = strdup(optarg);
        *(args.employeeName + len)='\0';
        break;
      case 'G':
      	if (args.employeeName != NULL ){
      		goto INVALID;
      	}
      	len = strlen(optarg);
        args.guestName = (char *) malloc(len + 1);
        args.guestName = strdup(optarg);
        *(args.guestName + len)='\0';
        break;
      case 'A':
    	if( args.eventDeparture ){
    		goto INVALID;
    	}
        args.eventArrival++;
        break;
      case 'L':
      	if( args.eventArrival ){
      		goto INVALID;
      	}
        args.eventDeparture++;
        break;
      case 'R':
        args.roomID = atoi(optarg);
        break;
      case '?':
        if (optopt == 'c')
          debug ("Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          debug("Unknown option `-%c'.\n", optopt);
        else
          debug ("Unknown option character `\\x%x'.\n",
                   optopt);
        goto INVALID;
      default:
        abort ();
      }
  index = optind;
  if(index < argc){
	  args.logName = argv[index];
  }
  debug("\nTimestamp: %d\nToken: %s\nEmployee: %s\nGuest: %s\nArrival: %d\nDeparture: %d\nRoomID: %d\nLogName: %s\nBatchName: %s",
		  args.timestamp, args.token, args.employeeName, args.guestName, args.eventArrival, args.eventDeparture,
		  args.roomID, args.logName, args.batchFile);
  status = 0;
  return args;

  INVALID: ;
  debug("\nINVALID");
  status = 0;
  return args;
}
