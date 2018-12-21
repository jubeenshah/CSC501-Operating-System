/* evec.c -- initevec, doevec */

#include <conf.h>
#include <i386.h>    
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <tty.h>
#include <q.h>
#include <io.h>
#include <stdio.h>

/*#define STKTRACE*/
/*#define REGDUMP*/

/*
 * The girmask is used as a mask for interrupts that don't have a
 * handler set. disable() & restore() are OR-ed with it to get the
 * mask actually used.
 */
short	girmask;

extern struct idt idt[NID];
extern struct segtr idtr;

extern long	defevec[];
extern int	userret();
extern int init8259();
extern int lidt();

int initevec()
{
	int		i;

	for (i=0; i<NID; ++i)
		set_evec(i, (long)defevec[i]);
	/*
	 * "girmask" masks all (bus) interrupts with the default handler.
	 * initially, all, then cleared as handlers are set via set_evec()
	 */
	girmask = 0xfffb;	/* leave bit 2 enabled for IC cascade */

	lidt();
	init8259();
        return(OK);
}

/*------------------------------------------------------------------------
 * set_evec - set exception vector to point to an exception handler
 *------------------------------------------------------------------------
 */
int set_evec(unsigned int xnum, u_long handler)
{
	struct	idt	*pidt;

	pidt = &idt[xnum];
	pidt->igd_loffset = handler;
	pidt->igd_segsel = 0x8;		/* kernel code segment */
	pidt->igd_mbz = 0;
	pidt->igd_type = IGDT_TRAPG;
	pidt->igd_dpl = 0;
	pidt->igd_present = 1;
	pidt->igd_hoffset = handler >> 16;
	if (xnum > 31 && xnum < 48) {
		/* enable the interrupt in the global IR mask */
		xnum -= 32;
		girmask &= ~(1<<xnum);
	}
        return(OK);
}

char *inames[17] = {
	"divided by zero",
	"debug exception",
	"NMI interrupt",
	"breakpoint",
	"overflow",
	"bounds check failed",
	"invalid opcode",
	"coprocessor not available",
	"double fault",
	"coprocessor segment overrun",
	"invalid TSS",
	"segment not present",
	"stack fault",
	"general protection violation",
	"page fault",
	"coprocessor error",
};

static long *fp;
/*------------------------------------------------------------------------
 * trap -- print some debugging info when a trap occurred 
 * Note: Need some more work.
 *------------------------------------------------------------------------
*/
#define REGDUMP

void trap(int inum)
{
	long		*sp;
	STATWORD 	PS;
	extern int	console_dev;

	disable(PS);
	asm("movl	%ebp,fp");
	sp = fp + 15;	/* eflags/CS/eip/ebp/regs/trap#/Xtrap/ebp */
	kprintf("Xinu trap!\n");
	if (inum < 16)
		kprintf("exception %d (%s) currpid %d (%s)\n", inum,
			inames[inum], currpid, proctab[currpid].pname);
	else
		kprintf("exception %d currpid %d (%s)\n", inum, currpid, 
			proctab[currpid].pname);
#ifdef REGDUMP
	kprintf("eflags %X ", *sp--);
	sp--;	/* skip OLD CS */
	kprintf("eip %X ", *sp);
	sp--;
	if (inum == 8 || (inum >= 10 && inum <= 14)) {
		kprintf("error code %08x (%u)\n", *sp, *sp);
		sp--;
	}
	sp--;	/* skip FP for Xint call */
	kprintf("register dump:\n");
	kprintf("eax %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ecx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebx %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("esp %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("ebp %08X (%u)\n", *sp, *sp);
	fp = sp;
	sp--;
	kprintf("esi %08X (%u)\n", *sp, *sp);
	sp--;
	kprintf("edi %08X (%u)\n", *sp, *sp);
	sp--;
#endif // REGDUMP
#ifdef STKTRACE
	stacktrace(currpid);
#endif // STKTRACE

	kprintf("\nTrap to monitor.....\n");
        while (1)
                monitor(devtab[console_dev].dvcsr);
}
