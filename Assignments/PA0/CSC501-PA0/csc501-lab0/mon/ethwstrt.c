#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moneepro.h>
#include <stdio.h>

/*#define DEBUG*/

/*------------------------------------------------------------------------
 * mon_ethwstrt - start output on an Intel EtherExpress
 *------------------------------------------------------------------------
 */
int mon_ethwstrt(ped)
struct ethdev    *ped;
{
    u_short		iobase, io_port, addr_port;
    struct ep           *pep;
    u_short		avail, len;
    u_short		end;	/* ending address of TX block */
    
#ifdef DEBUG
    kprintf("[eep_wstrt]");
#endif
        
    iobase = ped->ed_iobase;
    io_port = iobase + EEP_IO_PORT;
    addr_port = iobase + EEP_HOST_ADDRESS_REG;

    while (1) {
	/*
         * determine the available space of the transmit buffer
         */
        avail = ped->ed_txbuf_size - (ped->ed_tx_end - ped->ed_tx_start);
        if (avail > ped->ed_txbuf_size)
            avail -= ped->ed_txbuf_size;

#ifdef DEBUG
	kprintf("tx_start:%d, tx_end=%d, avail=%d\n",
		ped->ed_tx_start, ped->ed_tx_end, avail);
#endif

	/*
	 * peek the first packet in the queue
	 */
	pep = (struct ep *)mon_headq(ped->ed_outq);
	if (pep == 0) {
#ifdef DEBUG
	    kprintf("eep_wstrt: empty queue\n");
#endif
	    return(OK);		/* done */
	}

		/* to word boundary + header */
	len = (((pep->ep_len + 1) >> 1) << 1) + EEP_TX_HEADER;

#ifdef DEBUG
	kprintf("eep_wstrt: avail=%d, len=%d\n", avail, len);
#endif

	if (len > avail) {
#ifdef DEBUG
	    kprintf("eep_wstrt: no TX buffer space\n");
#endif
	    return(OK);
	}

	end = ped->ed_tx_end + len;

	/*
	 * check if the packet plus an extra header will wrap
	 * around on the TX buffer
	 */
	if ((end + EEP_TX_HEADER) >= ped->ed_txbuf_size) { /* wrapped */
	    end = end + EEP_TX_HEADER - ped->ed_txbuf_size;
	}

#ifdef DEBUG
	kprintf("eep_wstrt: tx_start=%d, tx_end=%d, end=%d\n",
		ped->ed_tx_start, ped->ed_tx_end, end);
#endif

	pep = (struct ep *)mon_deq(ped->ed_outq);
	
	/*
	 * append a TX block to the end
	 */
	outw(addr_port, ped->ed_tx_end);
	outw(io_port, EEP_XMT_CMD);	/* event */
	outw(io_port, 0);		/* status */
	outw(io_port, end);		/* next block */
	outw(io_port, pep->ep_len);	/* frame size */
	outsw(io_port, (int) &pep->ep_eh, len >> 1); /* copy packet */

	if (ped->ed_tx_start == ped->ed_tx_end) {
	    /*
	     * This is the very first TX block
	     */
	    outw(iobase + EEP_XMT_BAR, ped->ed_tx_end);
	    outb(iobase, EEP_XMT_CMD);
	}
	else {
	    /*
	     * if this is not the very first TX block, chain it.
	     */
	    outw(addr_port, ped->ed_tx_chain_bit);
	    outw(io_port, ped->ed_tx_chain_cnt | I82595_CHAIN_BIT);

	    /*
	     * clear the TX interrupt bit
	     */
	    outb(iobase + EEP_STATUS_REG, EEP_TX_INT);
	    outb(iobase, EEP_RESUME_XMT_CMD);
	}

	ped->ed_tx_chain_bit = ped->ed_tx_end + I82595_TX_COUNT_OFF;
	if (ped->ed_tx_chain_bit >= ped->ed_txbuf_size)
	    ped->ed_tx_chain_bit -= ped->ed_txbuf_size;
	    
	ped->ed_tx_chain_cnt = pep->ep_len;
	ped->ed_tx_end = end;

	freebuf(pep);

#ifdef DEBUG
	kprintf("eep_wstrt: chain=%d, chain_c=%d, st=%d, end=%d, avail=%d\n",
	ped->ed_tx_chain_bit, ped->ed_tx_chain_cnt, ped->ed_tx_start, 
	ped->ed_tx_end, avail);
#endif
    } /* while */
}
