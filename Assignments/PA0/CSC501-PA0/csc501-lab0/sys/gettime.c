/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include "lab0.h"
#include <proc.h>
#include <stdio.h>


/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */

extern int getutim(unsigned long *);
extern int summarizeProcess;

SYSCALL	gettime(long *timvar)
{
    /* long	now; */

	/* FIXME -- no getutim */

	unsigned int timeTaken;
        unsigned int startTime = ctr1000;
        if(!summarizeProcess) {

                globalTableSystemCall[currpid][GETTIME].systemCallName = "sys_gettime";
                globalTableSystemCall[currpid][GETTIME].systemCallFrequency = globalTableSystemCall[currpid][GETTIME].systemCallFrequency + 1;
        }

	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][GETTIME].systemCallAverageExecTime = globalTableSystemCall[currpid][GETTIME].systemCallAverageExecTime + timeTaken;
    return OK;
}
