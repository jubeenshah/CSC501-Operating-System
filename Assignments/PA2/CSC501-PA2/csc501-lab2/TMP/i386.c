/* i386.c - setsegs */

#include <icu.h>
#include <i386.h>
#include <kernel.h>


#define BOOTP_CODE
#define	KCODE	1
#define	KSTACK	2
#define	KDATA	3
#define TWOTEN 1024

struct sd gdt_copy[NGD] = {
		/* 0th entry NULL */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
		/* 1st, Kernel Code Segment */
	{ 0xffff, 0, 0, 6, 1, 1, 0, 1, 0xf, 0, 0, 1, 1, 0 },
		/* 2nd, Kernel Data Segment */
	{ 0xffff, 0, 0, 2, 0, 1, 0, 1, 0xf, 0, 0, 1, 1, 0 },
		/* 3rd, Kernel Stack Segment */
	{ 0xffff, 0, 0, 2, 0, 1, 0, 1, 0xf, 0, 0, 1, 1, 0 },
		/* 4th, Bootp Code Segment */
	{ 0xffff, 0, 0, 6, 1, 1, 0, 1, 0xf, 0, 0, 1, 1, 0 },
		/* 5th, Task for Xinu */
	{ 0xffff, 0, 0, 1, 1, 0, 0, 1, 0xf, 1, 0, 0, 1, 0 },
		/* 6th, Task for Interrupt 14 */
	{ 0xffff, 0, 0, 1, 1, 0, 0, 1, 0xf, 1, 0, 0, 1, 0 },
		/* Ignore This One */
	{ 0xffff, 0, 0, 2, 0, 1, 0, 1, 0xf, 0, 0, 1, 1, 0 },

};

/* Two Tasks for Final Project . First is for Xinu.
   Second is for interrupt handler.
*/
struct tss i386_tasks[2] = {

  {0,	/* previous task	*/
   0,	/* mbz			*/
   0,	/* esp0			*/
   0x18,/* ss0			*/
   0,	/* mbz			*/
   0,	/* esp1			*/
   0x18,/* ss1			*/
   0,	/* mbz			*/
   0,	/* esp2			*/
   0x18,/* ss2			*/
   0,	/* mbz			*/
   0,	/* cr3 (PDBR)		*/
   0,	/* eip			*/
   0,	/* efl			*/
   0,	/* eax			*/
   0,	/* ecx			*/
   0,	/* edx			*/
   0,	/* ebx			*/
   0,	/* esp			*/
   0,	/* ebp			*/
   0,	/* esi			*/
   0,	/* edi			*/
   0,	/* es			*/
   0,	/* mbz			*/
   0x8,	/* cs			*/
   0,	/* mbz			*/
   0x18,/* ss			*/
   0,	/* mbz			*/
   0x10,/* ds			*/
   0,	/* mbz			*/
   0,	/* fs			*/
   0,	/* mbz			*/
   0,	/* gs			*/
   0,	/* mbz			*/
   0,	/* local desc tab sel	*/
   0,	/* mbz			*/
   0,	/* debug		*/
   0,	/* mbz			*/
   0},	/* iomap base address	*/

  {0,	/* previous task	*/
   0,	/* mbz			*/
   0,	/* esp0			*/
   0x18,/* ss0			*/
   0,	/* mbz			*/
   0,	/* esp1			*/
   0x18,/* ss1			*/
   0,	/* mbz			*/
   0,	/* esp2			*/
   0x18,/* ss2			*/
   0,	/* mbz			*/
   0,	/* cr3 (PDBR)		*/ /* ! */
   0,	/* eip			*/ /* ! */
   0,	/* efl			*/
   0,	/* eax			*/
   0,	/* ecx			*/
   0,	/* edx			*/
   0,	/* ebx			*/
   0,	/* esp			*/ /* ! */
   0,	/* ebp			*/
   0,	/* esi			*/
   0,	/* edi			*/
   0,	/* es			*/
   0,	/* mbz			*/
   0x8,	/* cs			*/
   0,	/* mbz			*/
   0x18,/* ss			*/
   0,	/* mbz			*/
   0x10,/* ds			*/
   0,	/* mbz			*/
   0,	/* fs			*/
   0,	/* mbz			*/
   0,	/* gs			*/
   0,	/* mbz			*/
   0,	/* local desc tab sel	*/
   0,	/* mbz			*/
   0,	/* debug		*/
   0,	/* mbz			*/
   0},	/* iomap base address	*/
};

extern struct sd gdt[];
extern struct segtr gdtr;

long	initsp;		/* initial SP for nulluser() */

extern	char	*maxaddr;

/*------------------------------------------------------------------------
 * setsegs - initialize the 386 processor
 *------------------------------------------------------------------------
 */
setsegs()
{
	extern int	start, etext;
	struct sd	*psd;
	unsigned int	np, npages, lostk, limit;

	npages = sizmem();
/*
	maxaddr = (char *)(npages * NBPG - 1);
*/
	maxaddr = (char *)( TWOTEN * NBPG - 1); /* 10M size */
				 	      /* the top 10M is used for backing store */

	psd = &gdt_copy[1];	/* kernel code segment */
	np = ((int)&etext + NBPG-1) / NBPG;	/* # code pages */
	psd->sd_lolimit = np;
	psd->sd_hilimit = np >> 16;
#if 0
	psd = &gdt_copy[2];	/* kernel data segment */
	psd->sd_lolimit = npages;
	psd->sd_hilimit = npages >> 16;

	psd = &gdt_copy[3];	/* kernel stack segment */
	psd->sd_lolimit = npages;
	psd->sd_hilimit = npages >> 16;
#endif

	psd = &gdt_copy[4];	/* bootp code segment */
	psd->sd_lolimit = npages;   /* Allows execution of 0x100000 CODE */
	psd->sd_hilimit = npages >> 16;

	/* Set the descriptors for the tasks */
	/* Main Xinu task */

	psd = &gdt_copy[5];
	psd->sd_hibase = ((unsigned int) i386_tasks) >> 24;
	psd->sd_midbase = (((unsigned int) i386_tasks)>>16) & 0xff;
	psd->sd_lobase = ((unsigned int) i386_tasks) & 0xffff;

	/* Page Fault handler task */

	psd = &gdt_copy[6];
	psd->sd_hibase = ((unsigned int) & i386_tasks[1]) >> 24;
	psd->sd_midbase = (((unsigned int) & i386_tasks[1]) >> 16) & 0xff;
	psd->sd_lobase = ((unsigned int) & i386_tasks[1]) & 0xffff;

	blkcopy(gdt, gdt_copy, sizeof(gdt_copy));

	/* initial stack must be in physical
	   memory.
	*/
/*
	initsp = npages*NBPG  - 4;
*/
	initsp = 1024*NBPG  - 4;
}

/*------------------------------------------------------------------------
 * init8259 - initialize the 8259A interrupt controllers
 *------------------------------------------------------------------------
 */
init8259()
{
	STATWORD	PS;

	/* Master device */
	outb(ICU1, 0x11);	/* ICW1: icw4 needed		*/
	outb(ICU1+1, 0x20);	/* ICW2: base ivec 32		*/
	outb(ICU1+1, 0x4);	/* ICW3: cascade on irq2	*/
	outb(ICU1+1, 0x1);	/* ICW4: buf. master, 808x mode */
	outb(ICU1, 0xb);	/* OCW3: set ISR on read	*/

	/* Slave device */
	outb(ICU2, 0x11);	/* ICW1: icw4 needed		*/
	outb(ICU2+1, 0x28);	/* ICW2: base ivec 40		*/
	outb(ICU2+1, 0x2);	/* ICW3: slave on irq2		*/
	outb(ICU2+1, 0xb);	/* ICW4: buf. slave, 808x mode	*/
	outb(ICU2, 0xb);	/* OCW3: set ISR on read	*/

	disable(PS);
}

pseg(psd)
struct sd	*psd;
{
	int		i;
	unsigned char	*pb = (unsigned char *)psd;
	unsigned int	base;
	unsigned int	limit;

	kprintf("pseg (%X)\n", pb);
	kprintf("hex:   %02x", (unsigned int)*pb++);
	for(i=1; i<8; ++i) {
		kprintf(" (%X)", pb);
		kprintf(" %02x", (unsigned int)*pb++);
	}
	limit = ((int)psd->sd_hilimit) << 16;
	limit |= psd->sd_lolimit;
	base = ((int)psd->sd_hibase) << 24;
	base |= ((int)psd->sd_midbase)<< 16;
	base |= psd->sd_lobase;

	kprintf("\nby field: base %X limit %d perm %d\n", base, limit,
		psd->sd_perm);
	kprintf("iscode %d isapp %d dpl %d present %d avl %d\n",
		psd->sd_iscode, psd->sd_isapp, psd->sd_dpl, psd->sd_present,
		psd->sd_avl);
	kprintf("mbz %d 32b %d gran %d\n", psd->sd_mbz, psd->sd_32b,
		psd->sd_gran);
}
