/* ttyopen.c - ttyopen */

#include <conf.h>
#include <kernel.h>
#include <tty.h>

/*------------------------------------------------------------------------
 * ttyopen - associate a tty with a hardware device
 *------------------------------------------------------------------------
 */
int ttyopen(pdev, dvnum)
struct devsw	*pdev;
int		dvnum;
{
	struct tty	*ptty=NULL, *ttynew();

	if (pdev->dvioblk)	/* already open */
	{
	        if (ptty->tty_iflags & TIF_EXCLUSIVE)
			return SYSERR;
		else
			return OK;
	}
	ptty = ttynew();
	if (ptty == 0)
		return SYSERR;
	pdev->dvioblk = (char *)ptty;
	ptty->tty_pdev = (struct devsw *)pdev;
	ptty->tty_phw = (struct devsw *) &devtab[dvnum];
	devtab[dvnum].dvioblk = (char *)pdev;
	ptty->tty_iflags = ptty->tty_oflags = 0;
	ptty->tty_uid = -1;
	return OK;
}
