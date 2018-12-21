//Added functionalities for EXPDISTSCHED and LINUXSCHED
//resched.c
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include <math.h>

#define  SETONE 	1
#define  SETHALF	0.5
#define  SETZERO	0

unsigned long currSP;
extern int ctxsw(int, int, int, int);

/*-----------------------------------------------------------------------
 *  * resched  --  reschedule processor to highest priority ready process
 *   *
 *    * Notes:	Upon entry, currpid gives current process id.
 *     *		Proctab[currpid].pstate gives correct NEXT state for
 *      *			current process if other than PRREADY.
 *       *------------------------------------------------------------------------
 *        */

int resched()
{
	//pointer to old process entry
	register struct	pentry	*optr;
	//pointer to new process entry
	register struct	pentry	*nptr;
	//Declaring Variables for a forwardPass, iterationcounter, goodnessValue, and varaible for storing the exponential probability
	int iterationCounter = SETZERO;
	int forwardPass = SETONE;
	int nextProcess,maximumGoodness,exponentialSchedPriority;
	optr= &proctab[currpid];
	int classToBeScheduledCheck = getschedclass();

	switch (classToBeScheduledCheck) {
		case EXPDISTSCHED:
		//check if classToBeScheduledCheck is 1
		exponentialSchedPriority=expdev(0.1);
		//assign in a priority to exponentialSchedPriority
				 if (optr->pstate == PRCURR){
					 //check if the state is '\001'
					 	/* For reference
					 		#define	PRCURR		'\001'		 process is currently running
					 		#define	PRREADY		'\003'		 process is on ready queue
					 	*/
					//if so change the old process pointesr's process state to PRREADY
					// '\003'
					optr->pstate = PRREADY;
					/*int insert(int proc, int head, int key);*/
					// proc --> currpid, head --> the head, and the key is the process priority
					// currpid --> currently executing process
					insert(currpid,rdyhead,optr->pprio);
				}
				/* reference
				int	qkey;		key on which the queue is ordered/
				int	qnext;		pointer to next process or tail
				int	qprev;		pointer to previous process or head
				*/

				//select Next process
				nextProcess = q[rdyhead].qnext;
					while (q[nextProcess].qkey <(int) exponentialSchedPriority){
						//Repeat until the priority of the current process is less than exponentialSchedPriority
						nextProcess = q[nextProcess].qnext;
				}
				if(nextProcess>NPROC) {
				//int	qprev;		pointer to previous process or head
				nextProcess=q[rdytail].qprev;
			}
			//Updating the values for new process pointer
					currpid = nextProcess;
					nptr = &proctab[currpid];
					//mark it currently running
					nptr->pstate = PRCURR;
			//dequeing the process
			//int dequeue(int item);
					dequeue(nextProcess);
					//reset the preemption counter --> 10 the default clcok ticsk until preemption
					#ifdef	RTCLOCK
						preempt = QUANTUM;
					#endif
				break;
				//check if classToBeScheduledCheck is 2
			case LINUXSCHED:
			forwardPass=SETONE;
			proctab[currpid].quantum=preempt;
			if(proctab[currpid].quantum != SETZERO){

				// int currentProcessPriority = proctab[currpid].pprio;
				// int currentProcessQuantum = proctab[currpid].quantum;
				// proctab[currpid].goodness = currentProcessPriority + currentProcessQuantum;
				proctab[currpid].goodness=proctab[currpid].pprio+proctab[currpid].quantum;



			}
			else {

					proctab[currpid].goodness=SETZERO;

			}

			iterationCounter = SETZERO;
			do {
			//	iterationCounter = iterationCounter + SETONE;

			// int checkProcessState = proctab[iterationCounter].pstate;
			// int checkProcessQuantum =  proctab[iterationCounter].quantum;
			//if ((checkProcessState == PRREADY	|| checkProcessState == PRCURR) && checkProcessQuantum != 0) {
				if( ( proctab[iterationCounter].pstate==PRREADY || proctab[iterationCounter].pstate==PRCURR ) && proctab[iterationCounter].quantum!=SETZERO) {
					forwardPass = SETZERO;
				}
				iterationCounter = iterationCounter + SETONE;
			} while (iterationCounter < NPROC);

			if(forwardPass==SETONE){
				iterationCounter = SETZERO;
				do {
			//		iterationCounter = iterationCounter + SETONE;
					if(proctab[iterationCounter].pstate!=PRFREE) {
						 // 	int processQuantum = proctab[iterationCounter].quantum;
						 //   int processPriority = proctab[iterationCounter].pprio;
						 //   int processGoodness = proctab[iterationCounter].goodness;

						 // processQuantum = processPriority + (int) (SETHALF * procprocessQuantum);
						 // processGoodness = processPriority;
							proctab[iterationCounter].quantum=proctab[iterationCounter].pprio+(int)(SETHALF*proctab[iterationCounter].quantum);
							proctab[iterationCounter].goodness=proctab[iterationCounter].pprio;
					}
					iterationCounter = iterationCounter + SETONE;

				} while(iterationCounter < NPROC);
			}

			if (optr->pstate == PRCURR) {
				optr->pstate = PRREADY;
				insert(currpid,rdyhead,optr->pprio);
			}
			maximumGoodness = SETZERO;
			iterationCounter = SETZERO;
			do {
			//	iterationCounter = iterationCounter + SETONE;
				if((proctab[iterationCounter].pstate==PRREADY || proctab[iterationCounter].pstate==PRCURR) && proctab[iterationCounter].goodness>=maximumGoodness && proctab[iterationCounter].quantum!=SETZERO){
						maximumGoodness=proctab[iterationCounter].goodness;
						nextProcess=iterationCounter;
					}
				iterationCounter = iterationCounter + SETONE;
			} while(iterationCounter < NPROC);
			currpid = dequeue(nextProcess);
			nptr=&proctab[currpid];
			nptr->pstate = PRCURR;
			preempt = nptr->quantum;
			/*
			#ifdef	RTCLOCK
			preempt = QUANTUM;
			#endif
			*/
			break;

			default:
					optr= &proctab[currpid];

					if ( ( optr->pstate == PRCURR) &&
					   (lastkey(rdytail)<optr->pprio)) {
						return(OK);
					}
					if (optr->pstate == PRCURR) {
						optr->pstate = PRREADY;
						insert(currpid,rdyhead,optr->pprio);
					}
					currpid = getlast(rdytail);
					nptr = &proctab[ currpid ];
					nptr->pstate = PRCURR;
				#ifdef	RTCLOCK
				preempt = QUANTUM;
				#endif
				break;
			}

			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
			return OK;


	}
