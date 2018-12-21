#include <kernel.h>
#include "lock.h"
#include <proc.h>
#include <q.h>
#include <stdio.h>

#define SETONE  1
#define SETZERO 0
void release(int lock,int tmppid){
	int index = SETZERO;
    register struct lentry *lptr=&locks[lock];
    int newNReaders = lptr->nreaders++;
    int newNWriters = lptr->nwriters++;
	q[tmppid].qtype==READ?newNReaders:newNWriters;

    lptr->pidheld[tmppid]=index;
    proctab[currpid].lockheld[lock]=index;
    newlprio(lock);

    while (index < NPROC) {
      /* code */
      int checkPID = lptr->pidheld[index];
      if (checkPID == SETONE) {
        /* code */
        newpinh(index);
      }
      index = index + SETONE;
    }

    dequeue(tmppid);
    /* this process hold the lock and is put into ready list*/
    ready(tmppid,RESCHNO);

}

// /* release the specific one in the wait queue */
// #include<kernel.h>
// #include<proc.h>
// #include<stdio.h>
// #include<lock.h>
// #include<q.h>
//
// void release(int lock,int tmppid){
// 	int i;
//     register struct lentry *lptr=&locks[lock];
// 	q[tmppid].qtype==READ?lptr->nreaders++:lptr->nwriters++;
//
//     lptr->pidheld[tmppid]=1;
//     proctab[currpid].lockheld[lock]=1;
//     newlprio(lock);
//     for(i=0;i<NPROC;++i){
//       if(lptr->pidheld[i]==1){
//         newpinh(i);
//       }
//     }
//     dequeue(tmppid);
//     /* this process hold the lock and is put into ready list*/
//     ready(tmppid,RESCHNO);
//
// }
