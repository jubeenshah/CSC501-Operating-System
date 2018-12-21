/* i386.h - DELAY */

#define	NBPG		4096


#define	NID		48
#define	NGD		 8

#define	IRQBASE		32	/* base ivec for IRQ0			*/

/* Interrupt Descriptor		*/
/* See Intel Vol 3 Ch 5		*/

struct idt {
	unsigned short	igd_loffset;
	unsigned short	igd_segsel;
	unsigned int	igd_rsvd : 5;
	unsigned int	igd_mbz : 3;
	unsigned int	igd_type : 5;
	unsigned int	igd_dpl : 2;
	unsigned int	igd_present : 1;
	unsigned short	igd_hoffset;
};

#define	IGDT_TASK	 5	/* task gate IDT descriptor		*/
#define	IGDT_INTR	14	/* interrupt gate IDT descriptor	*/
#define	IGDT_TRAPG	15	/* Trap Gate				*/


/* Segment Descriptor	*/
/* See Intel Vol 3 Ch 3	*/
struct sd {
	unsigned short	sd_lolimit;
	unsigned short	sd_lobase;
	unsigned char	sd_midbase;
	unsigned int	sd_perm : 3;
	unsigned int	sd_iscode : 1;
	unsigned int	sd_isapp : 1;
	unsigned int	sd_dpl : 2;
	unsigned int	sd_present : 1;
	unsigned int	sd_hilimit : 4;
	unsigned int	sd_avl : 1;
	unsigned int	sd_mbz : 1;		/* must be '0' */
	unsigned int	sd_32b : 1;
	unsigned int	sd_gran : 1;
	unsigned char	sd_hibase;
};

/* Task State Segment	*/
/* See Intel Vol 3 Ch 6 */
struct tss {
  unsigned short	ts_ptl;
  unsigned short	ts_mbz12;
  unsigned int		ts_esp0;	/* use at pl 0 */
  unsigned short	ts_ss0;
  unsigned short	ts_mbz11;
  unsigned int		ts_esp1;	/* use at pl 1 */
  unsigned short	ts_ss1;
  unsigned short	ts_mbz10;
  unsigned int		ts_esp2;	/* use at pl 2*/
  unsigned short	ts_ss2;
  unsigned short	ts_mbz9;
  unsigned int		ts_pdbr;
  unsigned int		ts_eip;
  unsigned int		ts_efl;
  unsigned int		ts_eax;
  unsigned int		ts_ecx;
  unsigned int		ts_edx;
  unsigned int		ts_ebx;
  unsigned int		ts_esp;
  unsigned int		ts_ebp;
  unsigned int		ts_esi;
  unsigned int		ts_edi;
  unsigned short	ts_es;
  unsigned short	ts_mbz8;
  unsigned short	ts_cs;
  unsigned short	ts_mbz7;
  unsigned short	ts_ss;
  unsigned short	ts_mbz6;
  unsigned short	ts_ds;
  unsigned short	ts_mbz5;
  unsigned short	ts_fs;
  unsigned short	ts_mbz4;
  unsigned short	ts_gs;
  unsigned short	ts_mbz3;
  unsigned short	ts_ldtss;
  unsigned short	ts_mbz2;
  unsigned int		ts_t : 1;
  unsigned int		ts_mbz1 : 15;
  unsigned short	ts_ioba;
};

extern struct tss i386_tasks[];

#define	sd_type		sd_perm

/* System Descriptor Types */

#define	SDT_INTG	14	/* Interrupt Gate	*/

/* Segment Table Register */
struct segtr {
	unsigned int	len : 16;
	unsigned int	addr : 32;
};

/*
 * Delay units are in microseconds.
 */
#define	DELAY(n)					\
{							\
        extern int cpudelay;				\
        register int i;					\
	register long N = (((n)<<4) >> cpudelay);	\
 							\
	for (i=0;i<=4;i++)				\
	   {						\
	   N = (((n) << 4) >> cpudelay);		\
	   while (--N > 0) ;				\
	   }						\
}
