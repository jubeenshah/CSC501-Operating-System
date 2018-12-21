#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moneepro.h>
#include <stdio.h>

static int eep_xintr(struct ethdev *ped, u_short iobase);

/*#define DEBUG*/
/*------------------------------------------------------------------------
 * mon_ethintr - handle an EtherExpress PRO/10 device interrupt
 *------------------------------------------------------------------------
 */
int mon_ethintr()
{
    STATWORD    ps;
    struct 	ethdev	*ped;
    u_short	status;
    u_short	iobase;

    disable(ps);

    ped = &mon_eth[0];
    iobase = ped->ed_iobase;
    
    while (1) {
	status = inb(iobase + EEP_STATUS_REG);

#ifdef DEBUG
	kprintf("eep_intr: status=0x%x\n", status);
#endif
	
	status &= 0x0f;		/* get the lower byte */
	
	if (status == 0) {
#ifdef DEBUG
	    kprintf("eep_intr: exit >>>>\n");
#endif
	    restore(ps);	/* done */
	    return(OK);
	}

	/*
	 * process command complete interrupts if any
	 */
	if (status & EEP_EXEC_INT)
	    outb(iobase + EEP_STATUS_REG, EEP_EXEC_INT); /* ACK */
	    
	if (status & EEP_TX_INT) {
#ifdef DEBUG
	    kprintf("eep: TX int\n");
#endif

	    outb(iobase + EEP_STATUS_REG, EEP_TX_INT);	/* ACK */
	    
	    /* Process the status of transmitted packets */
	    eep_xintr(ped, iobase);
	}

	if (status & (EEP_RX_INT | EEP_RX_STP_INT)) {
#ifdef DEBUG
	    kprintf("eep: RX int\n");
#endif

	    outb(iobase + EEP_STATUS_REG, (EEP_RX_INT | EEP_RX_STP_INT));

	    /* Get the received packets */
	    mon_ethdemux(ped, iobase);
	}
    } /* while */
}

/*-------------------------------------------------------------------------
 * eep_xintr - handles a TX interrupt
 *-------------------------------------------------------------------------
 */
static int eep_xintr(struct ethdev *ped, u_short iobase)
{
    char restart = FALSE;
    u_short event, status, addr_port, io_port;

    addr_port = iobase + EEP_HOST_ADDRESS_REG;
    io_port = iobase + EEP_IO_PORT;

    while (ped->ed_tx_start != ped->ed_tx_end) {
	outw(addr_port, ped->ed_tx_start);
        event = inw(io_port);
#ifdef DEBUG_TX
	kprintf("eep_xintr: st=%d, TX event=0x%x\n", ped->ed_tx_start, event);
#endif
        if ((event & I82595_TX_DONE_BIT) == 0)
	    break;
	
	status = inw(io_port);
	
#ifdef DEBUG_TX
	kprintf("eep_xintr: status=%x, start=%x\n", status, ped->ed_tx_start);
#endif

	if (!(status & EEP_TX_OK)) {
	    restart = TRUE;
	    /*
	     * If we get here, we had a fatal transmit; we need to restart
	     * transmitter.
	     */
	}

	ped->ed_tx_start = inw(io_port);	/* next TX block */
	
	/*
	 * check to see if we need to restart transmitter
	 */
	if (restart) {
	    outw(iobase + EEP_XMT_BAR, ped->ed_tx_start);
	    if (ped->ed_tx_start != ped->ed_tx_end)
		outb(iobase, EEP_XMT_CMD);
	    restart = FALSE;
	}
    } /* while */

    /*
     * if packets waiting in the output queue, then transmit it
     */
    if (mon_lenq(ped->ed_outq))
        mon_ethwstrt(ped);
    return(OK);
}
