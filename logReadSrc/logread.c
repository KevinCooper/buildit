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
#include "htmlPrint.h"

void buildDataStructs(logappend_args *temp);
void doBadThings(logread_args* args);
void sortLinkedList_Names(Node *head);
void whyIsTheHTMLFormatDifferent_S(logread_args* args);

HT* allMahHashes;
Node *peopleHead;
int32_t highestRoomNum;
//
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
		currPerson->rooms = NULL;
		strcpy(currPerson->name, name);
		currPerson->isEmployee = isemployee;
		currPerson->roomID = -1;
		currPerson->inBuilding = 1;
		currPerson->enterTime = temp->timestamp;
		ht_put(allMahHashes, currPerson->name, currPerson);
		stack_push(&peopleHead, currPerson);
	} else if (temp->eventArrival == 1 && temp->roomID != -1) {
		currPerson->roomID = temp->roomID;
		int32_t * tempNum = malloc(sizeof(int32_t));
		*tempNum = temp->roomID;
		stack_push(&currPerson->rooms, tempNum);
		if (currPerson->roomID > highestRoomNum)
			highestRoomNum = currPerson->roomID;
	} else if (temp->eventDeparture == 1 && temp->roomID == -1) {
		currPerson->leaveTime = temp->timestamp;
		currPerson->inBuilding = 0;
	} else {
		currPerson->roomID = -1;
	}

}

void doBadThings(logread_args* args) {
	sortLinkedList_Names(peopleHead);
	int32_t currRoom;
	uint32_t isFirst = 1;
	if (args->currentState) {
		if (args->inHTML) {
			whyIsTheHTMLFormatDifferent_S(args);
			return;
		}
		Node* temp = peopleHead;
		while (temp) {
			person* tempP = (person *) (temp->data);
			if (tempP->isEmployee && tempP->inBuilding) {
				if (!isFirst)
					printf(",");
				isFirst = 0;
				printf("%s", tempP->name);
			}
			temp = temp->next;

		}
		isFirst = 1;
		temp = peopleHead;
		printf("\n");
		while (temp) {
			person* tempP = (person *) (temp->data);
			if (!tempP->isEmployee && tempP->inBuilding) {
				if (!isFirst)
					printf(",");
				isFirst = 0;
				printf("%s", tempP->name);
			}
			temp = temp->next;
		}
		printf("\n");
		for (currRoom = 0; currRoom <= highestRoomNum; currRoom++) {
			isFirst = 1;
			Node* temp = peopleHead;
			while (temp) {
				person* tempP = (person *) (temp->data);
				if (tempP->roomID == currRoom) {
					if (isFirst)
						printf("%d: ", currRoom);
					if (!isFirst)
						printf(",");
					isFirst = 0;
					printf("%s", tempP->name);

				}
				temp = temp->next;
			}
			if (!isFirst)
				printf("\n");
		}

	} else if (args->listAllRooms_R) {
		person* blahzz;
		if (args->employeeName != NULL) {
			blahzz = ht_get(allMahHashes, args->employeeName);
		} else {
			blahzz = ht_get(allMahHashes, args->guestName);
		}
		Node* temp = blahzz->rooms;
		reverse(&temp);
		uint32_t isFirst = 1;
		if (args->inHTML)
			init_R();
		while (temp) {
			int32_t* num = (int32_t*) (temp->data);
			if (args->inHTML) {
				print_R_element(num);
			} else {
				if (!isFirst)
					printf(",");
				isFirst = 0;
				printf("%d", *num);
			}
			temp = temp->next;
		}
		if (args->inHTML)
			printFooter();
	}
	//printf("\n");
	//fflush(stdout);

}

void columns(Node* temp_E, Node* temp_G) {
	person* tempE = temp_E == NULL ? NULL : (person *) (temp_E->data);
	person* tempG = temp_G == NULL ? NULL : (person *) (temp_G->data);
	if (!temp_E && !temp_G) {
		return;
	} else if (temp_E && !tempE->isEmployee) {
		columns(temp_E->next, temp_G);
	} else if (temp_G && tempG->isEmployee) {
		columns(temp_E, temp_G->next);
	} else if (temp_E == NULL && temp_G && !tempG->isEmployee) {
		printGallery(tempE, tempG);
		if (temp_G->next != NULL)
			printEndTableNewTable();
		printEndTableNewTable();
		columns(temp_E, temp_G->next);
	} else if (temp_G == NULL && temp_E && tempE->isEmployee) {
		printGallery(tempE, tempG);
		if (temp_E->next != NULL)
			printEndTableNewTable();
		columns(temp_E->next, temp_G);
	} else {
		printGallery(tempE, tempG);
		if (temp_E->next != NULL && temp_G->next != NULL)
			printEndTableNewTable();
		columns(temp_E->next, temp_G->next);
	}
}

void whyIsTheHTMLFormatDifferent_S(logread_args* args) {
	Node* temp_E = peopleHead;
	Node* temp_G = peopleHead;
	columns(temp_E, temp_G);
	uint32_t currRoom;
	printSetup_S_2();
	uint32_t isFirst;
	for (currRoom = 0; currRoom <= highestRoomNum; currRoom++) {
		isFirst = 1;
		Node* temp = peopleHead;
		while (temp) {
			person* tempP = (person *) (temp->data);
			if (tempP->roomID == currRoom) {
				if (isFirst)
					printf("<tr>\n\t<td>%d</td>\n\t<td>", currRoom);
				if (!isFirst)
					printf(",");
				isFirst = 0;
				printf("%s", tempP->name);

			}
			temp = temp->next;
		}
		if (!isFirst)
			printf("</td>\n</tr>\n");
	}

	printFooter();

}

void sortLinkedList_Names(Node *head) {
	Node *temp;
	int i;
	/* since the compare dereferences temp->next you'll have to verify that it is not NULL */
	for (i = 0; i < stack_len(head); i++) {
		for (temp = head; temp && temp->next; temp = temp->next) {
			person* blah = (person *) temp->data;
			person * blahNext = (person *) temp->next->data;
			if (strcmp(blah->name, blahNext->name) > 0) {
				/* no need for a whole node, since you only copy a pointer */
				person *cp;
				cp = temp->data;
				temp->data = temp->next->data;
				temp->next->data = cp;
			}

		}
	}

}
