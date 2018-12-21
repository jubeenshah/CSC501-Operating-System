/* open.c - open */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  open  -  open a connection to a device/file (parms 2 &3 are optional)
 *------------------------------------------------------------------------
 */
DEVCALL open(int descrp, int arg1, int arg2)
{
    struct devsw *devptr;

    if ( isbaddev(descrp) )
	return(SYSERR);
    devptr = &devtab[descrp];
    return((*devptr->dvopen)(devptr, arg1, arg2));
}
