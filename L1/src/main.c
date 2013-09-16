/*
 * CSC372 - Lab 1
 *
 * Main program
 */

// Project includes
#include "process.h"
#include "list.h"

// Platform includes
#include <stdio.h>
#include <stdlib.h>

//
// Provided example program
//
int main(int argc, char* argv[]){
	struct PD* pd, *pd2;
	struct LL* list;
	ListType type=L_PRIORITY;
	pd = AllocatePD();
	pd2 = AllocatePD();
	pd->pid = 1;
	pd->priority = 1;
	list = CreateList(type);
	PriorityEnqueue(pd,list);
	DequeuePD(pd);
	DestroyList(list);
	list = CreateList(L_WAITING);
	WaitlistEnqueue(pd, 10, list);
	pd2 = FindPD(1, list);
	if(!pd2) printf("test\n");
	DequeuePD(pd2);
	DestroyList(list) ;	
}
