/* comintr.c -- comintr */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <com.h>
#include <stdio.h>

/*#define DEBUG*/
/*------------------------------------------------------------------------
 *  comintr -- handle a serial line interrupt
 *------------------------------------------------------------------------
 */
int comintr()
{
    STATWORD		ps;
    unsigned char	iir, b;
    int			i, csr;

    disable(ps);
    
    for (i=0; i<Nserial; ++i) {
	csr = comtab[i].com_pdev->dvcsr;
	iir = inb(csr + UART_IIR);
	if (iir & UART_IIR_NO_INT)
	    continue;
	break;
    }

    if (i == Nserial) {
	restore(ps);
	return(OK);
    }

    switch (iir & UART_IIR_IID) {
    case UART_IIR_RLSI:
	b = inb(csr + UART_LSR);
#ifdef DEBUG
	kprintf("<LSR=%x>", b);
#endif
	if (b & UART_LSR_BI) {
	    (void) inb(csr + UART_RX);		/* discard it */

	    if (!(b & UART_LSR_OE)) { 		/* XXX */
		/* handles the BREAK signal here */
		kprintf("\nSerial line BREAK detected\n");
		monitor(csr);
	    }
	}
	break;
	    
    case UART_IIR_MSI:
	b = inb(csr + UART_MSR);
#ifdef DEBUG
	kprintf("(MSR=%x)", b);
#endif
	break;
	    
    case UART_IIR_RDI:		/* received a char */
	b = inb(csr + UART_RX);
#ifdef DEBUG
	kprintf("{RX=%x}", b);
#endif
	if (b == 0) {		/* XXX maybe a BREAK */
	    /* handles the BREAK signal here */
	    kprintf("\nSerial line BREAK detected.\n");
	    monitor(csr);
	}
	else
	    comiin(&comtab[i], b);
	break;
	    
    case UART_IIR_THRI:
	b = inb(csr + UART_LSR);
#ifdef DEBUG
	kprintf("[LSR=%x]", b);
#endif
	if (b & UART_LSR_THRE && comtab[i].com_count) {
	    comwstrt(&comtab[i], csr);
	}
	break;

    default:
	kprintf("comintr: unknown int: iir = 0x%x\n", iir);	
    }
    
    restore(ps);
    return(OK);
}

/*-------------------------------------------------------------------------
 * comwstrt - 
 *-------------------------------------------------------------------------
 */
int comwstrt(struct comsoft *pcom, int csr)
{
#ifdef DEBUG
    kprintf("comwstrt: ct=%d, st=%d", pcom->com_count, pcom->com_start); 
#endif
    outb(csr+UART_TX, pcom->com_buf[pcom->com_start]);
    pcom->com_count--;
    pcom->com_start++;
	
    if (pcom->com_count == 0)	/* disable tx ready interrupt */
	outb(csr+UART_IER, UART_IER_MSI | UART_IER_RLSI | UART_IER_RDI);

    if (pcom->com_start >= COMBUFSZ)
	pcom->com_start = 0;
    signal(pcom->com_osema);
    return OK;
}
