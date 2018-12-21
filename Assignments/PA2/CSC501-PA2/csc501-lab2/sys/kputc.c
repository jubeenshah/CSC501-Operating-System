/* kputc.c - kputc */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

extern int console_dev;
extern int comsputc(struct devsw *, unsigned char);
/*------------------------------------------------------------------------
 * kputc - do a synchronous kernel write to the console tty
 *------------------------------------------------------------------------
 */
int kputc(int dev, unsigned char c)
{
/*
    if (console_dev == KBMON)
	kbmputc(&devtab[console_dev], c);
    else
*/
	comsputc(&devtab[console_dev], c);
        return(OK);
}
