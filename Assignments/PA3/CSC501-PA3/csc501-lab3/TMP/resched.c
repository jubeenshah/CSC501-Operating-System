#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

#define SETONE	1
#define SETZERO	0
unsigned long currSP;
extern int ctxsw(int, int, int, int);
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int setTMPid = q[rdytail].qprev;
	int tmppid=setTMPid;
	int	reschedpid;
	int reschedprio=-SETONE;
	while(tmppid!=rdyhead){

	int checkPinH = proctab[tmppid].pinh;
		if(checkPinH==SETZERO){
			int checkPrio = proctab[tmppid].pprio;
			if(checkPrio > reschedprio){
				reschedprio=checkPrio;
				reschedpid=tmppid;
			}
		}
		else{
			if(checkPinH>reschedprio){
				reschedprio=checkPinH;
				reschedpid=tmppid;
			}
		}
		tmppid=q[tmppid].qprev;
	}

	int checkPstate = (optr= &proctab[currpid])->pstate;
	int checkPinHPrio = (optr->pinh==SETZERO?optr->pprio:optr->pinh);
	if ( ( checkPstate == PRCURR) && reschedprio<checkPinHPrio) {
		return(1);
	}

	/* force context switch */

	int checkPstateVal = optr->pstate;
	if ( checkPstateVal == PRCURR) {
		optr->pstate = PRREADY;
		int optrPprio = optr->pprio;
		insert(currpid,rdyhead,optrPprio);
	}

	int setCurrentPid = reschedpid;
	currpid= setCurrentPid;
	nptr=&proctab[setCurrentPid];

	nptr->pstate = PRCURR;		/* mark it currently running	*/
	dequeue(setCurrentPid);

#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif

	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

	/* The OLD process returns here when resumed. */
	return 1;
}
