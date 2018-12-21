/* freemem.c - freemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <stdio.h>
#include "lab0.h"
#include <proc.h> //currpid
/*------------------------------------------------------------------------
 *  freemem  --  free a memory block, returning it to memlist
 *------------------------------------------------------------------------
 */
extern int summarizeProcess;

SYSCALL	freemem(struct mblock *block, unsigned size)
{
	STATWORD ps;    
	struct	mblock	*p, *q;
	unsigned top;
	
	unsigned int timeTaken;
	unsigned int startTime = ctr1000;
	if(!summarizeProcess) {

		globalTableSystemCall[currpid][FREEMEM].systemCallName = "sys_freemem";
		globalTableSystemCall[currpid][FREEMEM].systemCallFrequency = globalTableSystemCall[currpid][FREEMEM].systemCallFrequency + 1;

	}
	
	if (size==0 || (unsigned)block>(unsigned)maxaddr
	    || ((unsigned)block)<((unsigned) &end)){
		timeTaken = ctr1000 - startTime;
		globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime = globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	size = (unsigned)roundmb(size);
	disable(ps);
	for( p=memlist.mnext,q= &memlist;
	     p != (struct mblock *) NULL && p < block ;
	     q=p,p=p->mnext )
		;
	if (((top=q->mlen+(unsigned)q)>(unsigned)block && q!= &memlist) ||
	    (p!=NULL && (size+(unsigned)block) > (unsigned)p )) {
		restore(ps);
		timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime = globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime + timeTaken;
		return(SYSERR);
	}
	if ( q!= &memlist && top == (unsigned)block )
			q->mlen += size;
	else {
		block->mlen = size;
		block->mnext = p;
		q->mnext = block;
		q = block;
	}
	if ( (unsigned)( q->mlen + (unsigned)q ) == (unsigned)p) {
		q->mlen += p->mlen;
		q->mnext = p->mnext;
	}
	restore(ps);
	timeTaken = ctr1000 - startTime;
                globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime = globalTableSystemCall[currpid][FREEMEM].systemCallAverageExecTime + timeTaken;
	return(OK);
}
