/* comiin.c comiin */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <com.h>

/*------------------------------------------------------------------------
 *  comiin  --  lower-half com device driver for input interrupts
 *------------------------------------------------------------------------
 */
INTPROC	comiin(struct comsoft * pcom, unsigned char ch)
{
    struct devsw	*pdev = pcom->com_pdev;

    if (pdev->dvioblk == 0)
	return(OK);		/* no tty device associated */
    
    pdev = (struct devsw *)pdev->dvioblk;	/* get tty dev pointer */
    
    if (pdev == 0)
	return(OK);		/* no tty structure associated */
    
    (pdev->dviint)(pdev, ch);
    return(OK);
}
