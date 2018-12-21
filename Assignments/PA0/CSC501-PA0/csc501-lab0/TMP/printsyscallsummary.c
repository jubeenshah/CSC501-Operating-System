#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include "lab0.h"

#define NOSYSCALL 27
int summarizeProcess = TRUE;

void syscallsummary_stop() {
	summarizeProcess = TRUE;
}

void syscallsummary_start() {
	summarizeProcess = FALSE;
}

void printsyscallsummary() {
	kprintf("\n\nvoid printsyscallsummary()");
	
	int currentProcess = 0;
	
	while(currentProcess < NPROC) {
	//	kprintf(" currentProcess : %d currentSystemCall : %d \n",currentProcess, currentSystemCall);			
		int currentSystemCall = 0;
//	kprintf(" currentProcess : %d currentSystemCall : %d \n",currentProcess, currentSystemCall);
		currentProcess = currentProcess + 1;
		if(traceProcessFlag[currentProcess] == TRUE){
		
			kprintf("\nProcess [pid:%d]",currentProcess);
	//		if (globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency > 0){	
			while (currentSystemCall < NOSYSCALL) {
	
	int averageExecutionTime;
	if (globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency > 0){  
				//float averageExecutionTime;
				if (globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency > 0) {

					averageExecutionTime = globalTableSystemCall[currentProcess][currentSystemCall].systemCallAverageExecTime / globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency;

				} else {

					averageExecutionTime = 0;

				}
				//printf(globalTableSystemCall[currentProcess][currentSystemCall].systemCallName);
				kprintf("\n\tSyscall: %s,",globalTableSystemCall[currentProcess][currentSystemCall].systemCallName);
				kprintf(" count: %d,",globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency);
				kprintf(" average execution time: %d (ms)", averageExecutionTime);

			//	currentSystemCall = currentSystemCall + 1;
			} else {
			//	kprintf("\nElse\tSyscall: %s,",globalTableSystemCall[currentProcess][currentSystemCall].systemCallName);
                         //       kprintf(" count: %d,",globalTableSystemCall[currentProcess][currentSystemCall].systemCallFrequency);
                           //     kprintf(" average execution time: %2.2lu", averageExecutionTime);
			}
		//	kprintf(" currentProcess : %d currentSystemCall : %d \n",currentProcess, currentSystemCall);
			
			
			currentSystemCall= currentSystemCall + 1;
		}

	//	currentProcess = currentProcess + 1;
	}
//	currentProcess = currentProcess + 1;
}
}

