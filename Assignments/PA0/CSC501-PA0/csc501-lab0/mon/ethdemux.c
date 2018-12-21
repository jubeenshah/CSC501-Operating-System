#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moneepro.h>
#include <stdio.h>

/*#define DEBUG*/
/*#define PRINTERR*/

static int rcv_frame(struct ethdev *ped, u_short port, u_short len);

#define MAX_RCV		10	/* max. # of frames received in one shot */
/*------------------------------------------------------------------------
 * mon_ethdemux - receive frames from an Intel EtherExpress
 *------------------------------------------------------------------------
 */
int mon_ethdemux(struct ethdev *ped, u_short iobase)
{
    u_short port = iobase + EEP_IO_PORT;
    u_short rcv_event, rcv_status, rcv_next, rcv_size, rcv_stop;
    int max_rcv = MAX_RCV;

#ifdef DEBUG
    kprintf("eep_demux: >>> start=%d\n", ped->ed_rx_start);
#endif

    while (1) {
	outw(iobase + EEP_HOST_ADDRESS_REG, ped->ed_rx_start);
	rcv_event = inw(port);
        rcv_status = inw(port);

	if (!((rcv_status & EEP_RX_OK) && (rcv_event & EEP_RCV_DONE)))
	    break;
	
        rcv_next = inw(port);
        rcv_size = inw(port);
	
#ifdef DEBUG
	kprintf("eep_demux: status=%x, next=%d, size=%d\n", rcv_status,
		rcv_next, rcv_size);
#endif
	    
	ped->ed_rx_start = rcv_next;

	if (rcv_size <= EP_MAXLEN) {
	    /*
	     * Note: may cause context switch
	     */
	    rcv_frame(ped, port, rcv_size);

	}
#ifdef PRINTERR
	else
	    kprintf("eep_demux: frame size too big! (len=%d)\n", rcv_size);
#endif
	
	/*
	 * set RX STOP address; make sure it won't fall in the TX buffer
	 */
	rcv_stop = ped->ed_rx_start - 2;
	if (rcv_stop < ped->ed_txbuf_size)
	    rcv_stop += ped->ed_rxbuf_size;
	
	outw(iobase + EEP_RCV_STOP, rcv_stop);
	
	if (--max_rcv == 0)
	    break;
    } /* while */

#ifdef DEBUG
    kprintf("eep_demux: exit start=%d\n", ped->ed_rx_start);
#endif
    return(OK);
}

/*-------------------------------------------------------------------------
 * rcv_frame - 
 *-------------------------------------------------------------------------
 */
static int rcv_frame(struct ethdev *ped, u_short port, u_short len)
{
    struct ep   *pep;
    
    pep = (struct ep *)nbgetbuf(mon_bufpool);
    if (pep == 0) {
#ifdef PRINTERR
        kprintf("rcv_frame: ?? no buffer\n");
#endif
        return(OK);
    }

#ifdef DEBUG
    kprintf("rcv_frame: len=%d\n", len);
#endif

    pep->ep_len = len;

    /*
     * copy packet to buffer
     */
    insw(port, (int) &pep->ep_eh, (len + 1) >> 1);
    pep->ep_type = net2hs(pep->ep_type);
    
#ifdef DEBUG
    {
        int i;

        kprintf("ETHER: dst %02x", pep->ep_dst[0]);
        for (i=1; i<EP_ALEN; ++i)
            kprintf(":%02x", pep->ep_dst[i]);
        kprintf(" src %02x", pep->ep_src[0]);
        for (i=1; i<EP_ALEN; ++i)
            kprintf(":%02x", pep->ep_src[i]);
        kprintf("\nETHER: type %x\n", pep->ep_type);
    }
#endif

    /*
     * pass it to upper layer; may cause context switch
     */
    if (mon_nif[0].ni_state == NIS_DOWN)
	freebuf(pep);
    else {
	/*
	 * may cause context switch
	 */
	mon_ni_in(&mon_nif[0], pep, pep->ep_len);
    }
    return(OK);
}
