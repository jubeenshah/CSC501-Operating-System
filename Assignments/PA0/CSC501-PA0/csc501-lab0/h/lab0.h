extern long zfunction(long param);	//Task 1
extern long printsegaddress();	//Task 2
extern long printtos();		//Task 3 
extern long printprocstkss(int); 	//Task 4

/**************************************************************************
 *									  *
 *					Task 5				  *
 *									  *
 *************************************************************************/
extern void printsyscallsummary();	//Printing the summary
extern void syscallsummary_start(); 	//setting traceProcessFlag[] as 1
extern void syscallsummary_stop();	//setting traceProcessFlag[] as 0
extern unsigned long ctr1000; 		//Global variable for time in XINU

extern int traceProcessFlag[];		//Creating a trace variable for all processes

struct systemCallSummary{ 		//Custom DS for storing the summary values globally
	unsigned int 	systemCallFrequency; 
	unsigned int 	systemCallAverageExecTime; 
	char 	       *systemCallName;
	}globalTableSystemCall[50][27]; 
//SYSCALL	freemem(struct mblock *block, unsigned size);
//SYSCALL chprio(int pid, int newprio);
//SYSCALL getpid();
//SYSCALL getprio(int pid);
//SYSCALL	gettime(long *timvar);
//SYSCALL kill(int pid);
//SYSCALL	receive();
//SYSCALL	recvclr();
//SYSCALL	recvtim(int maxwait);
//SYSCALL resume(int pid);
//SYSCALL scount(int sem);
//SYSCALL sdelete(int sem);
//SYSCALL	send(int pid, WORD msg);
//SYSCALL	setdev(int pid, int dev1, int dev2);
//SYSCALL	setnok(int nok, int pid);
//SYSCALL screate(int count);
//SYSCALL signal(int sem);
//SYSCALL signaln(int sem, int count);
//SYSCALL	sleep(int n);
//SYSCALL	sleep10(int n);
//SYSCALL sleep100(int n);
//SYSCALL sleep1000(int n);
//SYSCALL sreset(int sem, int count);
//SYSCALL stacktrace(int pid);
//SYSCALL	suspend(int pid);
//SYSCALL	unsleep(int pid);
//SYSCALL	wait(int sem);

#define FREEMEM 	0
#define CHPRIO		1
#define GETPID 		2
#define GETPRIO		3
#define GETTIME		4
#define KILL		5
#define RECEIVE		6
#define RECVCLR		7
#define RECVTIM		8
#define RESUME		9
#define SCOUNT		10
#define SDELETE		11
#define SEND		12
#define SETDEV		13
#define SETNOK		14
#define SCREATE		15
#define SIGNAL		16
#define SIGNALN		17
#define SLEEP		18
#define SLEEP10		19
#define SLEEP100	20
#define SLEEP1000	21
#define SRESET		22
#define STACKTRACE	23
#define SUSPEND		24
#define UNSLEEP		25
#define WAIT		26
