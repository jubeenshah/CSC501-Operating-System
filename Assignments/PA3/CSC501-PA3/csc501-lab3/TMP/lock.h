#ifndef _LOCK_H_
#define _LOCK_H_

#ifndef NLOCKS
#define NLOCKS 50
#endif


#define READ 1
#define WRITE 2


#define LFREE '\01'
#define LUSED '\02'
#define LDELETED '\03'


#define isbadlock(l) (l<0 || l>=NLOCKS)

struct lentry{
	char lstate;
	int ltype;
	int lqhead;
	int lqtail;
	int nreaders;
	int nwriters;
	int lprio;
	int pidheld[NPROC];
};

extern struct lentry locks[];
extern unsigned long ctr1000;

extern void linit();
extern int lcreate();
extern int ldelete(int lockdescriptor);
extern int lock(int ldes1,int type,int priority);
extern int releaseall(int numlocks,int ldes1, ...);

extern void newlprio(int lock);
extern void newpinh(int pid);

extern int lock_err(int ldes);
extern int lockaround;
extern int nextlock;

extern void admiReaders(int lock);
extern void release(int lock,int tmppid);

#endif
