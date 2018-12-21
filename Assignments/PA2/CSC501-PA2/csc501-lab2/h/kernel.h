/* kernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef _KERNEL_H_
#define _KERNEL_H_

/* Symbolic constants used throughout Xinu */

typedef	char		Bool;		/* Boolean type			*/
#define	FALSE		0		/* Boolean constants		*/
#define	TRUE		1
#define	EMPTY		(-1)		/* an illegal gpq		*/
#define	NULL		0		/* Null pointer for linked lists*/
#define	NULLCH		'\0'		/* The null character		*/
#define	NULLSTR		""		/* Pointer to empty string	*/
#define	SYSCALL		int		/* System call declaration	*/
#define	DEVCALL		int		/* Device call declaration	*/
#define	LOCAL		static 		/* Local procedure declaration	*/
#define	COMMAND		int		/* Shell command declaration	*/
#define	BUILTIN		int		/* Shell builtin " "		*/
#define	INTPROC		int		/* Interrupt procedure  "	*/
#define	PROCESS		int		/* Process declaration		*/
#define	RESCHYES	1		/* tell	ready to reschedule	*/
#define	RESCHNO		0		/* tell	ready not to resch.	*/
#define	MININT		0x80000000
#define	MAXINT		0x7fffffff
#define	LOWBYTE		0377		/* mask for low-order 8 bits	*/
#define	HIBYTE		0177400		/* mask for high 8 of 16 bits	*/
#define	LOW16		0177777		/* mask for low-order 16 bits	*/
#define	MINSTK		4096		/* minimum process stack size	*/
#define	NULLSTK		MINSTK		/* process 0 stack size		*/
#define	MAGIC		0125251		/* unusual value for top of stk	*/

/* Universal return constants */

#define	OK		 1		/* system call ok		*/
#define	SYSERR		-1		/* system call failed		*/
#define	EOF		-2		/* End-of-file (usu. from read)	*/
#define	TIMEOUT		-3		/* time out  (usu. recvtim)	*/
#define	INTRMSG		-4		/* keyboard "intr" key pressed	*/
					/*  (usu. defined as ^B)	*/
#define	BLOCKERR	-5		/* non-blocking op would block	*/
#define DELETED         -6              
#define DELETEME        -7

/* Initialization constants */

#define	INITSTK		1024		/* initial process stack size	*/
#define	INITPRIO	20		/* initial process priority	*/
#define	INITNAME	"main"		/* initial process name		*/
#define	INITARGS	1,0		/* initial count/arguments	*/
#define	INITRET		userret		/* processes return address	*/
#define	INITREG		0		/* initial register contents	*/
#define	QUANTUM		10		/* clock ticks until preemption	*/



/* Machine size definitions						*/

typedef	char	CHAR;		/* sizeof the unit the holds a character*/
typedef	int	WORD;		/* maximum of (int, char *)		*/
typedef	char	*PTR;		/* sizeof a char. or fcnt. pointer 	*/
typedef int	INT;		/* sizeof compiler integer		*/
typedef	int	REG;		/* sizeof machine register		*/

#define MAXLONG		0x7fffffff	
#define MINLONG		0x80000000

typedef short	STATWORD[1];	/* machine status for disable/restore	*/
				/* by declaring it to be an array, the	*/
				/* name provides an address so forgotten*/
				/* &'s don't become a problem		*/

/* Miscellaneous utility inline functions */
#define	isodd(x)	(01&(WORD)(x))
#define	min(a,b)	( (a) < (b) ? (a) : (b) )
#define	max(a,b)	( (a) > (b) ? (a) : (b) )

extern	int	rdyhead, rdytail;
extern	int	preempt;

/* Include types and configuration information */

#include <systypes.h>

#ifndef NDEVS
#include <conf.h>
#endif

/* ANSI compliant function prototypes */

int blkcmp(void *p1, void *p2, int len);
int blkcopy(void *to, void *from, int len);
int blkequ(void *p1, void *p2, int len);
void clkinit();
int dotrace(char *procname, int *argv, int argc);
int initevec();
int kputc(int dev, unsigned char c);
int main();
int panic(char *msg);
int ready(int pid, int resch);
int resched();
int set_evec(u_int xnum, u_long handler);
void trap(int inum);
int xdone();
long sizmem();

#include <mem.h>

WORD *getstk(unsigned int nbytes);
WORD *getmem(unsigned nbytes);
SYSCALL	freemem(struct mblock *block, unsigned size);

INTPROC	wakeup();

SYSCALL chprio(int pid, int newprio);
SYSCALL getpid();
SYSCALL getprio(int pid);
SYSCALL	gettime(long *timvar);
SYSCALL kill(int pid);
SYSCALL naminit();
SYSCALL	nammap(char *name, char *newname);
SYSCALL namopen(struct devsw *devptr, char *filenam, char *mode);
SYSCALL	namrepl(char *name, char *newname);
SYSCALL	pcount(int portid);
SYSCALL	pcreate(int count);
SYSCALL	pdelete(int portid, int (*dispose)());
SYSCALL	pinit(int maxmsgs);
SYSCALL	preceive(int portid);
SYSCALL	preset(int portid, int (*dispose)());
SYSCALL	psend(int portid, WORD msg);
SYSCALL premove(int portid, WORD msg);
SYSCALL pquery(int portid, WORD msg);
SYSCALL	receive();
SYSCALL	recvclr();
SYSCALL	recvtim(int maxwait);
SYSCALL	remove();
SYSCALL	rename(char *old, char *new);
SYSCALL resume(int pid);
SYSCALL scount(int sem);
SYSCALL sdelete(int sem);
SYSCALL	send(int pid, WORD msg);
SYSCALL	sendf(int pid, int msg);
SYSCALL	setdev(int pid, int dev1, int dev2);
SYSCALL	setnok(int nok, int pid);
SYSCALL screate(int count);
SYSCALL signal(int sem);
SYSCALL signaln(int sem, int count);
SYSCALL	sleep(int n);
SYSCALL	sleep10(int n);
SYSCALL sleep100(int n);
SYSCALL sleep1000(int n);
SYSCALL sreset(int sem, int count);
SYSCALL stacktrace(int pid);
SYSCALL	suspend(int pid);
SYSCALL	unsleep(int pid);
SYSCALL	wait(int sem);

int strtclk();
int stopclk();

/* Device I/O functions */

DEVCALL close(int descrp);
/*DEVCALL	control(int descrp, int func, int arg1, int arg2);*/
DEVCALL getc(int descrp);
DEVCALL init_dev(int descrp);
DEVCALL open(int descrp, int arg1, int arg2);
DEVCALL putc(int descrp, char ch);
DEVCALL read(int descrp, void *buf, int count);
DEVCALL seek(int descrp, long pos);
DEVCALL write(int descrp, void *buf, int count);

#endif
