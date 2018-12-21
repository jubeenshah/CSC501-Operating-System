/* cominit.c -- cominit */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <com.h>
#include <stdio.h>

struct comsoft	comtab[Nserial];
extern int delay(int);

/*------------------------------------------------------------------------
 *  cominit -- initialize a com device (NS16{4/5}50)
 *------------------------------------------------------------------------
 */
int cominit(struct devsw * pdev)
{
    struct 	comsoft	*pcom;
    int		comint();
    int		csr = pdev->dvcsr;

    pcom = &comtab[pdev->dvminor];
    pcom->com_pdev = pdev;
    pcom->com_osema = screate(COMBUFSZ);

    set_evec(pdev->dvivec, (u_long)comint);

    UART_OUT(csr+UART_LCR, UART_LCR_DLAB); 	/* access divisor latch */
    UART_OUT(csr+UART_DLL, 12);			/* 9600 baud */
    UART_OUT(csr+UART_DLM, 0);			/* 9600 baud */
    UART_OUT(csr+UART_LCR, UART_LCR_WLEN8);	/* 8N1 */
    
    comprobe(csr);

    UART_OUT(csr+UART_MCR, 0);
    (void)inb(csr + UART_MSR);

    /* raise DTR and RTS & enable interrupts */
    UART_OUT(csr+UART_MCR, UART_MCR_DTR | UART_MCR_RTS | UART_MCR_OUT2);
    (void)inb(csr + UART_MSR);

    UART_OUT(csr+UART_IER, UART_IER_MSI | UART_IER_RLSI | UART_IER_RDI);
    (void)inb(csr + UART_IIR);
    (void)inb(csr + UART_LSR);
    (void)inb(csr + UART_MSR);
    (void)inb(csr + UART_RX);
    return(OK);
}

/*-------------------------------------------------------------------------
 * comprobe - probe the com device
 *-------------------------------------------------------------------------
 */
int comprobe(int csr)
{
    char c1, c2;

    c1 = inb(csr+UART_LCR);
    UART_OUT(csr+UART_LCR, c1 | UART_LCR_DLAB);
    UART_OUT(csr+UART_EFR, 0); 	/* EFR is the same as FCR */
    UART_OUT(csr+UART_LCR, c1);
    UART_OUT(csr+UART_FCR, UART_FCR_ENABLE_FIFO);
    c2 = inb(UART_IIR) >> 6;
    
    switch (c2) {
    case 0:
	/* kprintf("UART = 16450\n"); */
	break;
	
    case 1:
	/* kprintf("UART = ??\n"); */
	break;
	
    case 2:
	/* kprintf("UART = 16550\n"); */
	break;
	
    case 3:
	UART_OUT(csr+UART_LCR, c1 | UART_LCR_DLAB);
	
	if (inb(csr+UART_EFR)) {
	    /* kprintf("UART = 16650\n"); */
	} else {
	    /* kprintf("UART = 16550A\n"); */
	}
	UART_OUT(csr+UART_LCR, c1);
	break;
    }

    UART_OUT(csr+UART_MCR, 0x00);
    UART_OUT(csr+UART_FCR, (UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT));
    (void)inb(csr+UART_RX);
    return(OK);
}

int comtest()
{
	kprintf("comtest\n");
        return(OK);
}

int comdump(int csr)
{
	int		i;
	unsigned char	lcr;

	kprintf("comdump: DLAB=0\n");
	lcr = inb(csr +UART_LCR);
	outb(csr +UART_LCR, lcr & ~UART_LCR_DLAB);
	for (i=0; i<8; ++i)
		kprintf("%02x ", inb(csr + i));
	kprintf("\nDLAB=1\n");
	outb(csr +UART_LCR, lcr | UART_LCR_DLAB);
	kprintf("%02x %02x\n", inb(csr), inb(csr + 1));
	outb(csr+UART_LCR, lcr);
        return(OK);
}

