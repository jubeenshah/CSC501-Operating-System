/* vcreate.c - vcreate */

#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
static unsigned long esp;
*/
#define SETONE  1
#define SETZERO 0
#define TWOTEN  1024


LOCAL	newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr,ssize,hsize,priority,name,nargs,args)
	int	*procaddr;		/* procedure address		*/
	int	ssize;			/* stack size in words		*/
	int	hsize;			/* virtual heap size in pages	*/
	int	priority;		/* process priority > 0		*/
	char	*name;			/* name (for debugging)		*/
	int	nargs;			/* number of args that follow	*/
	long	args;			/* arguments (treated like an	*/
					/* array in the code)		*/
{
	//kprintf(" VCREATE To be implemented!\n");

  STATWORD ps;
  disable(ps);
  int a = BACKING_STORE_BASE;
  int b = BACKING_STORE_UNIT_SIZE;
  int pid;
  int store;

  int checkStore;
  pid = create(procaddr, ssize, priority, name, nargs, args);
  checkStore = get_bsm(&store);

  if (checkStore == SYSERR) {
    restore (ps);
    return SYSERR;
  }
  int twoFourTen = TWOTEN * 4;
  bsm_map(pid, twoFourTen, store, hsize);
  bsm_tab[store].bs_private = SETONE;

  proctab[pid].vhpnpages = hsize;
  int list = getmem(sizeof(struct mblock *));
  proctab[pid].vmemlist = list;
  int next = (struct mblock *)(twoFourTen * twoFourTen);
  proctab[pid].vmemlist->mnext = next;
  proctab[pid].vmemlist->mlen = SETZERO;

  struct mblock *baseblock;
  int c = store * b;
  baseblock = a + c;
  baseblock->mlen = twoFourTen * hsize;
  baseblock->mnext = NULL;

  restore(ps);
	return pid;
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL	newpid()
{
	int	pid;			/* process id to return		*/
	int	i;

	for (i=0 ; i<NPROC ; i++) {	/* check all NPROC slots	*/
		if ( (pid=nextproc--) <= 0)
			nextproc = NPROC-1;
		if (proctab[pid].pstate == PRFREE)
			return(pid);
	}
	return(SYSERR);
}
