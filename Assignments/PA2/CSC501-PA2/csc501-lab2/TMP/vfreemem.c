/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>

#define SETZERO 0
#define SETONE  1
#define TWOTEN  1024

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL	vfreemem(block, size)
	struct	mblock	*block;
	unsigned size;
{
	//kprintf("VFREEMEM To be implemented!\n");
	STATWORD ps;
	struct mblock *index;
	unsigned top;
	struct mblock *indexDos;
	if (size == SETZERO) {
		/* code */
		return -SETONE;
	}

	size = (unsigned) roundmb(size);
	disable(ps);

	int list = &proctab[currpid].vmemlist;
	indexDos = list;

	index = indexDos->mnext;

	while (index != (struct mblock *)NULL && index< block) {
		indexDos = index;
		index = index->mnext;
	}


		int c = indexDos->mlen;
		int h = c + (unsigned)indexDos;
		top = h;
		int e = &proctab[currpid].vmemlist;
		int at = size + (unsigned)block;
		if ((top > (unsigned)block && indexDos != e) || (index != NULL && (at > (unsigned)index ) )) {
			/* code */
			restore(ps);
			return -SETONE;
		}
		int ed = (unsigned) block;

		if (indexDos != &memlist && top == ed) {
			/* code */
			int add = indexDos->mlen;
			indexDos->mlen =  add + size;
		} else {
			int s = size;
			block->mlen = s;
			block->mnext = index;
			indexDos->mnext = block;
			indexDos = block;
		}
		int a00 = indexDos->mlen;
		int a01 = (unsigned)(indexDos);
		int a02 = a00 + a01;
		int b00 = (unsigned)index;
		if (a02 == b00) {
			/* code */
			int addDos = indexDos->mlen;
			indexDos->mlen = addDos + index->mlen;
			indexDos->mnext = index->mnext;
		}

	restore(ps);
	return OK;
}
