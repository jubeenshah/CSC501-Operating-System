/* stacktrace.c - stacktrace */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

static unsigned long	*esp;
static unsigned long	*ebp;

#define STKDETAIL

/*------------------------------------------------------------------------
 * stacktrace - print a stack backtrace for a process
 *------------------------------------------------------------------------
 */
SYSCALL stacktrace(int pid)
{
	struct pentry	*proc = &proctab[pid];
	unsigned long	*sp, *fp;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][STACKTRACE].systemCallName = "sys_stacktrace";
                globalTableSystemCall[currpid][STACKTRACE].systemCallFrequency = globalTableSystemCall[currpid][STACKTRACE].systemCallFrequency + 1;

        }
	if (pid != 0 && isbadpid(pid)){
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime = globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime + timeTaken;
		return SYSERR;
	}
	if (pid == currpid) {
		asm("movl %esp,esp");
		asm("movl %ebp,ebp");
		sp = esp;
		fp = ebp;
	} else {
		sp = (unsigned long *)proc->pesp;
		fp = sp + 2; 		/* where ctxsw leaves it */
	}
	kprintf("sp %X fp %X proc->pbase %X\n", sp, fp, proc->pbase);
#ifdef STKDETAIL
	while (sp < (unsigned long *)proc->pbase) {
		for (; sp < fp; sp++)
			kprintf("DATA (%08X) %08X (%d)\n", sp, *sp, *sp);
		if (*sp == MAGIC)
			break;
		kprintf("\nFP   (%08X) %08X (%d)\n", sp, *sp, *sp);
		fp = (unsigned long *) *sp++;
		if (fp <= sp) {
			kprintf("bad stack, fp (%08X) <= sp (%08X)\n", fp, sp);
			//timeTaken = ctr1000 - startTime;
               // globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime = globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime + timeTaken;
			return SYSERR;
		}
		kprintf("RET  0x%X\n", *sp);
		sp++;
	}
	kprintf("MAGIC (should be %X): %X\n", MAGIC, *sp);
	if (sp != (unsigned long *)proc->pbase) {
		kprintf("unexpected short stack\n");
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime = globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime + timeTaken;
		return SYSERR;
	}
#endif
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime = globalTableSystemCall[currpid][STACKTRACE].systemCallAverageExecTime + timeTaken;
	return OK;
}
