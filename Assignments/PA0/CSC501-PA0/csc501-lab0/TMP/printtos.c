#include <stdio.h>

static unsigned long 	*ebp;

void printtos() {

	kprintf("\n\nvoid printtos()\n");
	
	asm("movl %ebp, ebp");
	long *prev, *curr;
	prev = ebp + 2;
	curr = ebp;
	
	kprintf("Before[0x%08x]: 0x%08x\n",prev, *prev);
	kprintf("After [0x%08x]: 0x%08x\n",curr, *curr);

	/*Printing upto four stack locations below the top of the stack */
	int index = 1;	 	//Starting index
	int noOfStack = 4;	//Number of Stacks needed
	while(index <= noOfStack) {

                kprintf("\telement[0x%08x] : 0x%08x\n", (curr-index),*(curr - index));
                index = index + 1;
        }
	
}
