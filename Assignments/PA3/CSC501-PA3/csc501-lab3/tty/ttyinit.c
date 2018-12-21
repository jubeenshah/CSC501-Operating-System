/* ttyinit.c - ttyinit */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

struct tty	ttytab[Ntty];

/*------------------------------------------------------------------------
 * ttyinit - initialize tty structures
 *------------------------------------------------------------------------
 */
int ttyinit(pdev)
struct devsw	*pdev;
{
    pdev->dvioblk = (char *)0;  /* in case Xinu restarts. John Lin 07/96 */ 
    ttytab[pdev->dvminor].tty_state = TTYS_FREE;
    return(OK);
}

