/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */

extern int summarizeProcess;

SYSCALL getpid()
{

	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][GETPID].systemCallName = "sys_getpid";
                globalTableSystemCall[currpid][GETPID].systemCallFrequency = globalTableSystemCall[currpid][GETPID].systemCallFrequency + 1;
        }

	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][GETPID].systemCallAverageExecTime = globalTableSystemCall[currpid][GETPID].systemCallAverageExecTime + timeTaken;
	return(currpid);
}
