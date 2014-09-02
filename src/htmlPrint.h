/*
 * htmlPrint.h
 *
 *  Created on: Sep 2, 2014
 *      Author: kevin
 */

#ifndef HTMLPRINT_H_
#define HTMLPRINT_H_

#include "definitions.h"
#include <stdio.h>


void printHeader();
void printFooter();
void init_R();
void print_R_element(int32_t * element);

#endif /* HTMLPRINT_H_ */
