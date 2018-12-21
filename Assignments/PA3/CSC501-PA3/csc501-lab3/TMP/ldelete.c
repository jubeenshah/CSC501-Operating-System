
#include <conf.h>
#include <kernel.h>
#include "lock.h"
#include <proc.h>
#include <q.h>
#include <stdio.h>

#define SETONE  1
#define SETZERO 0
#define SETSIX  6

SYSCALL ldelete(int lockdescriptor) {
STATWORD ps;

struct lentry *lptr;

int pid;
int lock = lockdescriptor / 10000;

disable(ps);

int ret = lock_err(lockdescriptor);

switch (ret) {
  case -SETONE:
        restore(ps);
        return (ret);
        break;
  case -SETSIX:
        restore(ps);
        return (ret);
        break;
}

lptr=&locks[lock];
lptr->lstate = '\03';

int emptyIS = lptr->lqhead;
if (nonempty(emptyIS)) {
  /* code */
  int emptyISAgain = lptr->lqhead;
  pid = getfirst(emptyISAgain);
  while (pid != (-1)) {
    /* code */
    ready(pid, 0);
  }
  resched();
}
restore(ps);
return(1);
}
