/* conf.h (GENERATED FILE; DO NOT EDIT) */

#define	NULLPTR	(char *)0

/* Device table declarations */
struct	devsw	{			/* device table entry */
	int	dvnum;
	char	*dvname;
	int	(*dvinit)();
	int	(*dvopen)();
	int	(*dvclose)();
	int	(*dvread)();
	int	(*dvwrite)();
	int	(*dvseek)();
	int	(*dvgetc)();
	int	(*dvputc)();
	int	(*dvcntl)();
	int	dvcsr;
	int	dvivec;
	int	dvovec;
	int	(*dviint)();
	int	(*dvoint)();
	char	*dvioblk;
	int	dvminor;
	};

extern	struct	devsw devtab[];		/* one entry per device */


/* Device name definitions */

#define	CONSOLE     0			/* type tty      */
#define	SERIAL0     1			/* type serial   */
#define	SERIAL1     2			/* type serial   */
#define	TTY0        3			/* type tty      */
#define	TTY1        4			/* type tty      */
#define	TTY2        5			/* type tty      */

/* Control block sizes */

#define	Nserial	2
#define	Ntty	4

#define	NDEVS	6

/* Declarations of I/O routines referenced */

extern	int	ttyinit();
extern	int	ttyopen();
extern	int	ionull();
extern	int	ttyread();
extern	int	ttywrite();
extern	int	ioerr();
extern	int	ttycntl();
extern	int	ttygetc();
extern	int	ttyputc();
extern	int	ttyiin();
extern	int	ttyoin();
extern	int	cominit();
extern	int	comwrite();
extern	int	comcntl();
extern	int	comgetc();
extern	int	computc();
extern	int	comint();


/* Configuration and Size Constants */

#define	LITTLE_ENDIAN	0x1234
#define	BIG_ENDIAN	0x4321

#define	BYTE_ORDER	LITTLE_ENDIAN

#define	NPROC	    50			/* number of user processes	*/
#define	NSEM	    100			/* number of semaphores		*/
#define	MEMMARK				/* define if memory marking used*/
#define	RTCLOCK				/* now have RTC support		*/
#define	STKCHK				/* resched checks stack overflow*/
