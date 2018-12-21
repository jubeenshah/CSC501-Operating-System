/* screate.c - screate, newsem */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;

LOCAL int newsem();

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
SYSCALL screate(int count)
{
	STATWORD ps;    
	int	sem;
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SCREATE].systemCallName = "sys_screate";
                globalTableSystemCall[currpid][SCREATE].systemCallFrequency = globalTableSystemCall[currpid][SCREATE].systemCallFrequency + 1;

        }
	disable(ps);
	if ( count<0 || (sem=newsem())==SYSERR ) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SCREATE].systemCallAverageExecTime = globalTableSystemCall[currpid][SCREATE].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	semaph[sem].semcnt = count;
	/* sqhead and sqtail were initialized at system startup */
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SCREATE].systemCallAverageExecTime = globalTableSystemCall[currpid][SCREATE].systemCallAverageExecTime + timeTaken;
	return(sem);
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newsem()
{
	int	sem;
	int	i;

	for (i=0 ; i<NSEM ; i++) {
		sem=nextsem--;
		if (nextsem < 0)
			nextsem = NSEM-1;
		if (semaph[sem].sstate==SFREE) {
			semaph[sem].sstate = SUSED;
			return(sem);
		}
	}
	return(SYSERR);
}
