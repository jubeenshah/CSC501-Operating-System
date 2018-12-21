/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include "lab0.h"
#include <proc.h>
extern int summarizeProcess;

/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
SYSCALL scount(int sem)
{
extern	struct	sentry	semaph[];

	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][SCOUNT].systemCallName = "sys_scount";
                globalTableSystemCall[currpid][SCOUNT].systemCallFrequency = globalTableSystemCall[currpid][SCOUNT].systemCallFrequency + 1;

        }
	if (isbadsem(sem) || semaph[sem].sstate==SFREE){
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SCOUNT].systemCallAverageExecTime = globalTableSystemCall[currpid][SCOUNT].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][SCOUNT].systemCallAverageExecTime = globalTableSystemCall[currpid][SCOUNT].systemCallAverageExecTime + timeTaken;
	return(semaph[sem].semcnt);
}
