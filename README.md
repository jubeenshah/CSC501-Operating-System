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
* [CH 15, 16](./Notes/03-12-2018/)
	* [Notes](./Notes/03-12-2018/CH15,16.pdf)
* [CH 19, 20, 21, 22](./Notes/04-12-2018/)
	* [Notes](./Notes/04-12-2018/CH19,20,21,22.pdf)
* [CH 26, 27, 28](./Notes/05-12-2018/)
	* [Notes](./Notes/05-12-2018/CH26,27,28.pdf)
* [CH 29, 30, 32, 36, 37](./Notes/06-12-2018/)
	* [Notes](./Notes/06-12-2018/CH29,30,32,36,37.pdf)
* [CH 38, 39, 40, 41](./Notes/07-12-2018/)
	* [Notes](./Notes/07-12-2018/CH38,39,40,41,42.pdf)
* [CH 42 -- VMM](./Notes/08-12-2018/)
	* [Notes](./Notes/08-12-2018/08-12-2018.pdf)
* [CH 50](./Notes/09-12-2018/)
	* [Notes](./Notes/09-12-2018/CH50.pdf)