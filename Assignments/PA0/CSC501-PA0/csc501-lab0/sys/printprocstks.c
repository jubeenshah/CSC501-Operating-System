#include <stdio.h>
#include <kernel.h>
#include "lab0.h"
#include <proc.h>
static unsigned long *esp; //Stack pointer access as defined in ../sys/stacktrace.c


void printprocstks(int priority){	

	struct pentry *process; /* for access to pentry from proc.h 
					process->pname  (process name)
					process->pbase  (stack base) 
					process->plimit (stack limit)
					process->pprio  (process priority)
					process->pstklen(stack length/size)
					process->pstate (process state) 
				*/
	kprintf("\nvoid printprocstks(int priority)");
	int processId = 0;
	int isCurrentProcess = 0;
	while (processId < NPROC) {
		++processId;
	process = &proctab[processId]; //from proc.h -> extern	struct pentry proctab[]
	
	if((process->pstate) == PRFREE || (process->pprio <= priority)) {
	/*Checking if the process is currently executing --> pstate == PFREE 
		i.e process slot is not empty
		and also checking if the process priority is greater than the parameter passed (Given in question)*/
	continue;
	}else{
	if (processId == currpid) {
		asm("movl %esp,esp"); //current process --> stacktrace.c
		isCurrentProcess = 1;
	} else {
		isCurrentProcess = 0;
	}
	
	kprintf("\nProcess [%s]",process->pname);	//process->pname  (process name)
	kprintf("\n\tpid: %d",processId);		//processId
	kprintf("\n\tpriority: %d",process->pprio);	//process->pprio  (process priority)
	kprintf("\n\tbase: 0x%08x",process->pbase);	//process->pbase  (stack base) 
	kprintf("\n\tlimit: 0x%08x",process->plimit);	//process->plimit (stack limit)
	kprintf("\n\tlen: %d",process->pstklen);		//process->pstklen(stack length/size)
	
	isCurrentProcess == 1 ? printf("\n\tpointer: 0x%08x",esp) : printf("\n\tpointer: 0x%08x",process ->pesp);
		
		}
	}	

}
