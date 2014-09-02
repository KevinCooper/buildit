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
#include "hash.h"
#include "dictionary.h"

void buildDataStructs(logappend_args *temp);
void doBadThings(logread_args* args);

HT* allMahHashes;
Node *peopleHead;

int main(int argc, char * argv[]) {
	peopleHead = NULL;
	allMahHashes = ht_create(65536);
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
		buildDataStructs(&temp);
		bzero(interString, 256);
		argv_free(tempv);
		// FINISH LOGICZ
	}
	doBadThings(&args);
	return 0;
}

void buildDataStructs(logappend_args *temp) {
	int32_t isemployee = 0;
	char * name = NULL;
	if (temp->employeeName == NULL) {
		name = temp->guestName;

	} else {
		name = temp->employeeName;
		isemployee = 1;
	}

	person* currPerson = ht_get(allMahHashes, name);
	if (currPerson == NULL) {
		currPerson = malloc(sizeof(person));
		strcpy(currPerson->name, name);
		currPerson->isEmployee = isemployee;
		if (temp->roomID != -1) {
			currPerson->roomID = temp->roomID;
		} else {
			currPerson->roomID = 0;
		}
		currPerson->enterTime = temp->timestamp;
		ht_put(allMahHashes, currPerson->name, currPerson);
		stack_push(&peopleHead, currPerson);
	} else if (temp->eventArrival == 1 && temp->roomID != -1) {
		currPerson->roomID = temp->roomID;
	} else if (temp->eventDeparture == 1 && temp->roomID == -1) {
		currPerson->leaveTime = temp->timestamp;
	} else {
		currPerson->roomID = 0;
	}

}

void doBadThings(logread_args* args) {
	tree_node *employees = NULL;
	tree_node *guests = NULL;
	Node* temp = peopleHead;
	if (args->currentState) {
		while (temp) {
			person* tempP = (person *) (temp->data);
			if (tempP->isEmployee) {
				insert(employees, tempP->name);
			} else {
				insert(guests, tempP->name);
			}
			temp = temp->next;
		}
	}
	printf("The employees are: ");
	print_tree(employees);
	printf("\nThe guests are: ");
	print_tree(guests);
	printf("\n");

}
