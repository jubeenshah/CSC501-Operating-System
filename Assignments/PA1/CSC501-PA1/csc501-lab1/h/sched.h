#define EXPDISTSCHED 	1
#define LINUXSCHED 	2

extern int 	classOfSchedulerToReturn;
extern void 	setschedclass (int classOfSchedulerToSet);
extern int 	getschedclass();
