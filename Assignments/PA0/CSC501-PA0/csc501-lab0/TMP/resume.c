/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"

extern int summarizeProcess;
/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
SYSCALL resume(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/
	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][RESUME].systemCallName = "sys_resume";
                globalTableSystemCall[currpid][RESUME].systemCallFrequency = globalTableSystemCall[currpid][RESUME].systemCallFrequency + 1;

        }
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RESUME].systemCallAverageExecTime = globalTableSystemCall[currpid][RESUME].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][RESUME].systemCallAverageExecTime = globalTableSystemCall[currpid][RESUME].systemCallAverageExecTime + timeTaken;
	return(prio);
}
