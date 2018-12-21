#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lock.h>
#include <stdio.h>
#include <lock.h>

#define SETONE  1
#define SETZERO 0
#define BIGVAL  100000000

void writer1(char message,int lockPass,int prio){
  kprintf(" %c : to acquire lock\n",message);
  lock(lockPass,WRITE,prio);
  kprintf(" %c : acquired lock \n",message);
  int index     = SETZERO;
  int indexDoes = SETZERO;
  while (index < BIGVAL) {
    /* code */
    index = index + SETONE;
    indexDos = indexDos + SETONE;
  }

  kprintf(" %c : to release lock\n",message);
  releaseall(SETONE,lockPass);
}

void writer2(char message,int semaphore){
  kprintf(" %c : to wait\n",message);
  wait(semaphore);
  kprintf(" %c : wait\n",message);
  int index     = SETZERO;
  int indexDoes = SETZERO;
  while (index < BIGVAL) {
    /* code */
    index = index + SETONE;
    indexDos = indexDos + SETONE;
  }
  kprintf(" %c : to signal\n",message);
  signal(semaphore);

}

void writer(char message){
  kprintf(" %c start to write\n",message);
  int index     = SETZERO;
  int indexDoes = SETZERO;
  while (index < BIGVAL) {
    /* code */
    index = index + SETONE;
    indexDos = indexDos + SETONE;
  }
  kprintf(" %c write done.\n",message);
}

int main(){
  kprintf("lock with priority inheritance\n");
  int lockPass=lcreate();
  int w1a=create(writer1,2000,20,"writer1",3,'W1A',lockPass,20);
  int w1b=create(writer1,2000,30,"writer1",3,'W1B',lockPass,20);
  int wclock=create(writer,2000,25,"writer",1,'W1C');
  resume(w1a);
  resume(w1b);
  resume(wclock);

  sleep(10);

  kprintf("semaphore without priority inheritance\n");
  int semaphore=screate(SETONE);
  int w2a=create(writer2,2000,20,"writer2",2,'W2A',semaphore);
  int w2b=create(writer2,2000,30,"writer2",2,'W2B',semaphore);
  int wcsem=create(writer,2000,25,"writer",1,'W2C');
  resume(w2a);
  resume(w2b);
  resume(wcsem);


  return SETZERO;
}
