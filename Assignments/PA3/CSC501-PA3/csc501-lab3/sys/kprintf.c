/* kprintf.c - kprintf */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

extern int disable(short *);
extern int restore(short *);
extern int _doprnt(char *, int *,  void *, int);
/*------------------------------------------------------------------------
 *  kprintf  --  kernel printf: formatted, unbuffered output to CONSOLE
 *------------------------------------------------------------------------
 */
int kprintf(char *fmt, int args)
{
	STATWORD	ps;

	disable(ps);
        _doprnt(fmt, &args, (void *)kputc, CONSOLE);
	restore(ps);
        return OK;
}
