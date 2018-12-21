/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include "lab0.h"
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int pidMain;
void create_process(void);
void createArbitraryProcess ();
int main()
{
	syscallsummary_start();
	pidMain = getpid();
	kprintf("\n\nProgramming Assignment 0\n\n");
	kprintf("\nZfunction(0xaabbccdd) --> 0x%08x\n", zfunction(0xaabbccdd));
	kprintf("\nZfunction(0x00000000) -->0x%08x\n",  zfunction(0x00000000));
	create_process();
	printsegaddress();	
	printtos();
	printprocstks(10);
	printsyscallsummary();
	syscallsummary_stop();
	return 0;
}

void createArbitraryProcess() {
	int lag_rand = rand() % 10;
	//printf("lag rand : %d", lag_rand);
	sleep(1);
	sleep1000(1);
	int currentpid = 0;

    	currentpid = getpid();
    	suspend(getpid());
   	setnok(pidMain, currentpid);
    	kill(currentpid);
}

void create_process(void) {

/*SYSCALL create(procaddr,ssize,priority,name,nargs,args)
 * 	int	*procaddr;		 procedure address		
	int	ssize;			 stack size in words		
	int	priority;		 process priority > 0		
	char	*name;			 name (for debugging)		
	int	nargs;			 number of args that follow	
	long	args;			 arguments (treated like an array in the code)
				
 * */
	resume(create((int *) &createArbitraryProcess, 1024, 40, "proc A", 1, 30));


}
