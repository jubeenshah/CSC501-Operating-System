/* init.c - init_dev */

#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  init_dev  -  initialize a device
 *------------------------------------------------------------------------
 */
DEVCALL init_dev(int descrp)
{
    struct	devsw	*pdev;

    if (isbaddev(descrp) ) {
	return(SYSERR);
    }
    pdev = &devtab[descrp];
    return((pdev->dvinit)(pdev));
}
