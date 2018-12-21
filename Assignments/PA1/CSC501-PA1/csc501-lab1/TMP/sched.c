#include <sched.h>
//#include <stdio.h>

int classOfSchedulerToReturn = 0; /*default*/
void setschedclass(int classOfSchedulerToSet) {
	classOfSchedulerToReturn = classOfSchedulerToSet;
} 

int getschedclass () {
	return classOfSchedulerToReturn;
}

