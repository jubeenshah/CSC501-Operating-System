/* io.h -  fgetc, fputc, getchar, isbaddev, putchar */

#ifndef _IO_H_
#define _IO_H_

#ifdef	NDEVS
#define	isbaddev(f)	( (f)<0 || (f)>=NDEVS )
#endif
#define	BADDEV		-1

/* In-line I/O procedures */

#define	getchar()	getc(CONSOLE)
#define	putchar(ch)	putc(CONSOLE,(ch))
#define	fgetc(unit)	getc((unit))
#define	fputc(unit,ch)	putc((unit),(ch))

/* Unix open flags */
#define	O_RDONLY	000		/* open for reading */
#define	O_WRONLY	001		/* open for writing */
#define	O_RDWR		002		/* open for read & write */
#define	O_NDELAY	00004		/* non-blocking open */
#define	O_APPEND	00010		/* append on each write */
#define	O_CREAT		01000		/* open with file create */
#define	O_TRUNC		02000		/* open with truncation */
#define	O_EXCL		04000		/* error on create if file exists */

#endif
