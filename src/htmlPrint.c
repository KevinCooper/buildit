
#include "htmlPrint.h"

void printHeader(){
	printf("<html>\n<body>\n<table>\n");
}
void printFooter(){
	printf("</table>\n</body>\n</html>\n");
}

void init_R(){
	printHeader();
	printf("<tr>\n<th>Rooms</th>\n</tr>\n");
}
void print_R_element(int32_t * element){
	printf("<tr>\n<th>%d</th>\n</tr>\n", *element);
}

