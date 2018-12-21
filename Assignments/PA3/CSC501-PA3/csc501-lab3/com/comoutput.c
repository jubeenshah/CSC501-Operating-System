/* comoutput.c - computc, comwrite */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <tty.h>
#include <com.h>
#include <stdio.h>

int comsputc(struct devsw * pdev, unsigned char c);
/*------------------------------------------------------------------------
 *  computc - write one character to the PC physical monitor
 *------------------------------------------------------------------------
 */
int computc(pdev, c)
struct devsw * pdev;
unsigned char c;
{
    STATWORD	ps;
    struct 	devsw	*pttydev;
    struct 	comsoft	*pcom = &comtab[pdev->dvminor];
    struct 	tty	*ptty=NULL;
    int		pos, rv;

    disable(ps);
    
    if ((pttydev = (struct devsw *)pdev->dvioblk))
	ptty = (struct tty *) pttydev->dvioblk;
    
    if (ptty && (ptty->tty_oflags & TOF_SYNC)) {
	/* comflush(pdev); */
	rv = comsputc(pdev, c);
	restore(ps);
	return rv;
    }

    wait(pcom->com_osema);
    
    pos = pcom->com_start + pcom->com_count;
    if (c == '\n') {
	wait(pcom->com_osema);	/* need 2 for \r\n */
	if (pos >= COMBUFSZ)
	    pos -= COMBUFSZ;
	pcom->com_buf[pos] = '\r';
	pcom->com_count++;
	++pos;
    }
    
    if (pos >= COMBUFSZ)
	pos -= COMBUFSZ;
    pcom->com_buf[pos] = c;
    pcom->com_count++;
    
    outb(pdev->dvcsr+UART_IER, UART_IER_ALLI);	/* enable tx ready interrupt */
    rv = inb(pdev->dvcsr + UART_LSR);
    if ((char)rv & UART_LSR_THRE) {
        comwstrt(&comtab[pdev->dvminor], pdev->dvcsr);
    }
    restore(ps);
    return OK;
}

/*------------------------------------------------------------------------
 * comflush - flush output buffer synchronously
 *------------------------------------------------------------------------
 */
int comflush(struct devsw * pdev)
{
    struct comsoft	*pcom = &comtab[pdev->dvminor];
    int		ier;
    int		csr = pdev->dvcsr;

    ier = inb(csr + UART_MCR);
    ier &= ~UART_IER_THRI;
    outb(csr + UART_IER, ier);
    while (pcom->com_count) {
	while ((inb(csr+UART_LSR) & UART_LSR_THRE) == 0)
	    /* empty */;
	outb(csr+UART_TX, pcom->com_buf[pcom->com_start]);
	pcom->com_start++;
	if (pcom->com_start >= COMBUFSZ)
	    pcom->com_start = 0;
	pcom->com_count--;
	signal(pcom->com_osema);
    }
    ier = inb(csr + UART_MCR);
    ier |= UART_IER_THRI;
    outb(csr + UART_IER, ier);
    return(OK);
}

/*------------------------------------------------------------------------
 * comsputc - do synchronous putc to a com device
 *------------------------------------------------------------------------
 */
int comsputc(struct devsw * pdev, unsigned char c)
{
    int csr = pdev->dvcsr;
    
    while ((inb(csr+UART_LSR) & UART_LSR_THRE) == 0)
	/* empty */;
    
    outb(csr+UART_TX, c);
    
    if (c == '\n') {
	while ((inb(csr+UART_LSR) & UART_LSR_THRE) == 0)
	    /* empty */;
	
	outb(csr+UART_TX, '\r');
    }
    return(OK);
}


/*------------------------------------------------------------------------
 *  comwrite - write to the PC physical monitor
 *------------------------------------------------------------------------
 */
int comwrite(struct devsw * pdev, char * buf, int count)
{
    if (count < 0)
	return SYSERR;
    
    if (count == 0)
	return OK;

    for (; count>0 ; count--)
	computc(pdev, *buf++);
    
    return OK;
}
