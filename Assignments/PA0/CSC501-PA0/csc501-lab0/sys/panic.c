/*------------------------------------------------------------------------
 *	panic  --  panic and abort XINU
 *------------------------------------------------------------------------
 */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*static unsigned long	esp, ebp;*/

int panic(char *msg)
{
	extern int console_dev;
	STATWORD ps;    
	disable(ps);
	kprintf("currpid %d (%s)\n", currpid, proctab[currpid].pname);
	kprintf("Panic: %s\n", msg);
/*
	asm("movl	%esp,esp");
	asm("movl	%ebp,ebp");
	stacktrace(esp, ebp);
*/
	while (1)
		monitor(devtab[console_dev].dvcsr);
	restore(ps);
}

