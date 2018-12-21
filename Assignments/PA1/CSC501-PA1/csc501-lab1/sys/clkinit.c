/* clkinit.c - clkinit, updateleds, dog_timeout */

#include <conf.h>
#include <kernel.h>
#include <sleep.h>
#include <i386.h>
#include <stdio.h>
#include <q.h>

/* Intel 8254-2 clock chip constants */

#define	CLOCKBASE	0x40		/* I/O base port of clock chip	*/
#define	CLOCK0		CLOCKBASE
#define	CLKCNTL		(CLOCKBASE+3)	/* chip CSW I/O port		*/
    
/* real-time clock variables and sleeping process queue pointers	*/
    
#ifdef	RTCLOCK
int	count6;			/* counts in 60ths of a second 6-0	*/
int	count10;		/* counts in 10ths of second 10-0	*/
unsigned long	ctr1000;	/* counts in 1000ths of second 0-INF	*/
unsigned long clktime = 0;	/* current time in seconds since 1/1/70	*/
int	clmutex;		/* mutual exclusion for time-of-day	*/
int     defclk;			/* non-zero, then deferring clock count */
int     clkdiff;		/* deferred clock ticks			*/
int     slnempty;		/* FALSE if the sleep queue is empty	*/
int     *sltop;			/* address of key part of top entry in	*/
				/* the sleep queue if slnonempty==TRUE	*/
int     clockq;			/* head of queue of sleeping processes  */
int	preempt;		/* preemption counter.	Current process */
				/* is preempted when it reaches zero;	*/
#ifdef	RTCLOCK
				/* set in resched; counts in ticks	*/
int	clkruns;		/* set TRUE iff clock exists by setclkr	*/
#else
int	clkruns = FALSE;	/* no clock configured; be sure sleep	*/
#endif				/*   doesn't wait forever		*/

/*
 *------------------------------------------------------------------------
 * clkinit - initialize the clock and sleep queue (called at startup)
 *------------------------------------------------------------------------
 */
void clkinit()
{
	unsigned short	intv;
	int clkint();

	set_evec(IRQBASE, (u_long)clkint);

	/* clock rate is 1.190 Mhz; this is 10ms interrupt rate */
	intv = 1190;

	clkruns = 1;
	clockq = newqueue();
	preempt = QUANTUM;		/* initial time quantum		*/
	clmutex = screate(1);

	/*  set to: timer 0, 16-bit counter, rate generator mode,
		counter is binary */
	outb(CLKCNTL, 0x34);
	/* must write LSB first, then MSB */
	outb(CLOCK0, (char)intv);
	outb(CLOCK0, intv>>8);
	outb(CLOCK0, intv>>8);
}
#endif


#ifdef notyet
/*
 * dog_timeout -- called when the watchdog timer determines that
 * interrupts have been disabled for too long
 */
dog_timeout()
{
    STATWORD ps;

    disable(ps);
    kprintf("\n\nWatchdog timeout!! -- interrupts disabled too long.\n");
    ret_mon();
    restore(ps);
    return;
}
#endif /* not yet */

int clktest()
{
	kprintf("from clkint, ctr1000 %d\n", ctr1000);
	kprintf("from clkint, clktime %d\n", clktime);
        return(OK);
}
