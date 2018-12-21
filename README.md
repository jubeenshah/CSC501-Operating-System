# CSC501-Operating-System

![](./10-NCSU-Logo.png)

#Contents
* [Assignments]()
* [Notes (OSTEP)]()
* [Cheat-Sheet]()
* [Remzi-Homework]()

#Assignments

* PA0 -- The objective of this introductory lab is to get familiarize with the process of compiling and running XINU, the tools involved, and the run-time environment and segment layout. (Scored 91)
	* Link -- [CSC501-PA0](./Assignments/PA0)
* PA1 -- The objective of this assignment is to get familiar with the concepts of process management, including process priorities, scheduling, and context switching. (Scored 91)
	* Link -- [CSC501-PA1](./Assignments/PA1)
* PA2 -- The goal of this project is to implement the following system calls and their supporting infrastructure.
	* SYSCALL xmmap (int virtpage, bsd\_t source, int npages)
	* SYSCALL xmunmap (int virtpage)
	* SYSCALL vcreate (int \*procaddr, int ssize, int hsize, int priority, char \*name, int nargs, long args)
	* WORD \*vgetmem (int nbytes)
	* SYSCALL srpolicy (int policy)
	* SYSCALL vfreemem (block\_ptr, int size\_in\_bytes)
	* Link -- [CSC501-PA2](./Assignments/PA2)
* PA3 -- In PA3, you are going to implement readers/writer locks as described in this handout. Additionally, you will implement a priority inheritance mechanism to prevent the priority inversion problem when using locks.
	* Link -- [CSC501-PA3](./Assignments/PA3)

# Notes

Notes are not ordered in a particular order. I just divided the notes in a way that helped me study for the final exam. The following are the links to the Chapters for OSTEP PDFs and then also the notes that I made for the respective chapters

* [Chapter 2, 4, 5, 6](./Notes/01-12-2018/)
	* [Notes](./Notes/01-12-2018/CH2,4,5,6.pdf)
* [CH 7, 8, 9, 13, 14](./Notes/02-12-2018/)
	* [Notes](./Notes/02-12-2018/CH 7, 8, 9, 13, 14.pdf)
* 