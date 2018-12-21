/* lcreate.c - lcreate, newlock  */

#include<kernel.h>
#include<proc.h>
#include<q.h>
#include<lock.h>
#include<stdio.h>

#define SETONE  1
#define SETZERO 0

LOCAL int newlock();

SYSCALL lcreate(){
	STATWORD ps;

	int lock;

	disable(ps);
  lock=newlock();
	if(lock == -1){
		restore(ps);
		return -1;
	}

	restore(ps);
	return (lock);
}

LOCAL int newlock(){
	int lock;
	int index;

  index = SETZERO;
  while (index < NLOCKS) {
    /* code */
    nextlock = nextlock - SETONE;
    lock=nextlock;
    if(nextlock < SETZERO){
      /* next NLOCKS around */
      nextlock   = NLOCKS - SETONE;
      lockaround = lockaround + SETONE;
    }
    int checkState = locks[lock].lstate;
    if(checkState != LUSED){
      locks[lock].lstate    = LUSED;
      locks[lock].nreaders  = SETZERO;
      locks[lock].nwriters  = SETZERO;
      int valToReturn       = lock * 10000;
      valToReturn           = valToReturn + lockaround;
      return (valToReturn);
    }
    index = index + SETONE;
  }
	return -1;
}
