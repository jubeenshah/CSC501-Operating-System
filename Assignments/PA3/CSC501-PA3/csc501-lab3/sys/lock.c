/* lock.c - lock*/

#include<kernel.h>
#include<proc.h>
#include<q.h>
#include<lock.h>
#include<stdio.h>

#define SETONE  1
#define SETZERO 0


SYSCALL lock(int ldes1, int type, int priority){
	STATWORD ps;

	/* help me check lock if this process need wait*/
	Bool needwait=0;

	int lock=ldes1/10000;

	struct lentry *lptr;
	struct pentry *pptr;

	int i;
  int lmaxprio;

	disable(ps);

	lptr=&locks[lock];
	/* lock is invalid or not created.  */
	int ret=lock_err(ldes1);;
	if(ret==-1||ret==-6){
		restore(ps);
		return (ret);
	}

  int checkNReaders = lptr->nreaders;
  int checkNWriters = lptr->nwriters;
	if(checkNReaders == SETZERO &&
     checkNWriters !=SETZERO){
		needwait=1;
		/* write lock here */
	}
	else if(checkNReaders != SETZERO &&
          checkNWriters == SETZERO &&
          type  ==(SETONE + SETONE)   ){
		needwait=1;
		/* read lock now but requested by write*/
	}
	else if(checkNReaders != SETZERO &&
          checkNWriters == SETZERO &&
          type==1){
					int setTail = lptr->lqtail;
		lmaxprio=q[setTail].qprev;
		while(priority<q[lmaxprio].qkey){
			if(q[lmaxprio].qtype==(SETONE + SETONE)){
				needwait=1;
				break;
			}
			lmaxprio=q[lmaxprio].qprev;
		}
	}
	pptr=&proctab[currpid];
	pptr->plockret=1;
	if(!needwait){
    type==1?lptr->nreaders++:lptr->nwriters++;
		lptr->pidheld[currpid]=SETONE;
		pptr->lockheld[lock]=SETONE;
    int pidToPass = currpid;
		newpinh(pidToPass);
	}
	else{
    pptr->pstate=PRLOCK;
		pptr->lockid=ldes1/10000;
		int passHead  = lptr->lqhead;
		insert(currpid,passHead,priority);

		q[currpid].qtype=type;
		q[currpid].qtime=ctr1000;

		pptr->plockret=1;

		/* update lprio in locks */
		newlprio(lock);

		/* update the pinh of the low priority process holding the lock */

		struct lentry * helplptr=&locks[lock];
    i = SETZERO;
    while (i < NPROC) {
      /* code */
      int checkPIDHeld = helplptr->pidheld[i];
      if(checkPIDHeld == SETONE){
				newpinh(i);
      }
       i = i + SETONE;
    }
		resched();

	}
	restore(ps);
  int returnValue = pptr->plockret;
	return (returnValue);

}

	/* lock is invalid or not created.  */
int lock_err(int ldes){
	int lock=ldes/10000;
	int lockard=ldes-lock*10000;
	register struct lentry *lptr=&locks[lock];
  int checkBadLock = isbadlock(lock);
  int checkLockState = lptr->lstate ;
	if( checkBadLock || checkLockState == LFREE || lockard!=lockaround) {
		return -1;
	}
	else if(checkLockState=='\03'){
		return -6;
	}
	return 1;
}
/* update the pinh of the low priority process holding the lock */

void newpinh(int pid){
	int i;
  int pmaxprio = -SETONE;
	int priocompare;
  int tmppid;
	register struct lentry *lptr;
	register struct pentry *pptr=&proctab[pid];
  i = SETZERO;
  while (i < NLOCKS) {
    /* code */
    if(proctab[pid].lockheld[i]==SETONE){
      lptr=&locks[i];
      int checkLprio = lptr->lprio;
      if(pmaxprio<checkLprio){
        pmaxprio=checkLprio;
      }
    }
    i = i + SETONE;
  }
  int setMaxPrio = pptr->pprio;
  if (setMaxPrio>pmaxprio) {
    /* code */
    proctab[pid].pinh = SETZERO;
  } else {
    proctab[pid].pinh = pmaxprio;
  }
}

void newlprio(int lock){
	int maxprio = -SETONE;
  int priocompare;
	struct lentry *tmplptr=&locks[lock];
  int lqtailSet = tmplptr->lqtail;
  int lqheadSet = tmplptr->lqhead;
	int curlockid=q[lqtailSet].qprev;
	while(curlockid!=lqheadSet){
    lqheadSet = tmplptr->lqhead;
		priocompare=(proctab[curlockid].pinh==SETZERO?proctab[curlockid].pprio:proctab[curlockid].pinh);
		if(priocompare>maxprio){
      int prioSet = priocompare;
			maxprio=prioSet;
		}
    int curLockIDSet = q[curlockid].qprev;
		curlockid=curLockIDSet;
	}
	tmplptr->lprio=maxprio;
}
