#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/montftp.h>
#include <./mon/monbootp.h>
#include <stdio.h>

/*#define PRINTERR*/
/*#define DEBUG*/

u_short mon_udpcksum(struct ep *pep, int len);

/*------------------------------------------------------------------------
 * mon_udp_in -  handle an inbound UDP datagram
 *------------------------------------------------------------------------
 */
int mon_udp_in(struct netif *pni, struct ep *pep)
{
    struct	ip	*pip = (struct ip *)pep->ep_data;
    struct	udp	*pudp = (struct udp *)pip->ip_data;
    int		ret;

    if (pudp->u_cksum && mon_udpcksum(pep, net2hs(pudp->u_len))) {
#ifdef PRINTERR
	kprintf("udp_in: !! UDP checksum error\n");
#endif
	freebuf(pep);
	return SYSERR;			/* checksum error */
    }

    /*
     * to host byte order
     */
    pudp->u_src = net2hs(pudp->u_src);
    pudp->u_dst = net2hs(pudp->u_dst);
    pudp->u_len = net2hs(pudp->u_len);

    /*
     * demux based on the destination port
     */
    switch (pudp->u_dst) {
    case BOOTP_CPORT:
#ifdef DEBUG
	kprintf("mon_udp_in: BOOTP in\n");
#endif
	if (mon_boot_state != BOOTP_REQ_SENT)
	    break;	/* ignore it */
	
	if (mon_timeout)
            mon_timeout = 0;        /* stop retx timer */

	if (mon_bootp_in(pudp) == OK) {
	   mon_boot_state = BEGIN_TFTP;
	   mon_boot_try = 0;
	}
	else
	    mon_boot_state = BOOTP_RETX;	/* try again */
	break;

    case TFTP_MY_TID:
#ifdef DEBUG
	kprintf("mon_udp_in: TFTP in\n");
#endif
	if (mon_boot_state == TFTP_REQ_SENT) {
	    mon_timeout = 0;        /* stop retx timer */
	    
	    ret = mon_tftp_in(pudp);
	    if (ret == OK)
		mon_boot_state = BOOT_DONE;
	    else if (ret == SYSERR) {
		mon_boot_state = TFTP_RETX;	/* try again */
	    }
	}
	break;

    default:
#ifdef DEBUG
	kprintf("mon_udp_in: discard\n");
#endif	
	break;
    }

    freebuf(pep);
    return OK;
}

/*------------------------------------------------------------------------
 *  mon_udpcksum -  compute a UDP pseudo-header checksum
 *------------------------------------------------------------------------
 */
u_short mon_udpcksum(struct ep *pep, int len)
{
	struct	ip	*pip = (struct ip *)pep->ep_data;
	struct	udp	*pudp = (struct udp *)pip->ip_data;
	unsigned	short	*sptr;
	unsigned	long ucksum;
	int		i;

	ucksum = 0;

	sptr = (unsigned short *) &pip->ip_src;
	/* 2*IP_ALEN octets = IP_ALEN shorts... */
	/* they are in net order.		*/
	for (i=0; i<IP_ALEN; ++i)
		ucksum += *sptr++;
	sptr = (unsigned short *)pudp;
	ucksum += hs2net(IPT_UDP + len);
	if (len % 2) {
		((char *)pudp)[len] = 0;	/* pad */
		len += 1;	/* for the following division */
	}
	len >>= 1;	/* convert to length in shorts */

	for (i=0; i<len; ++i)
		ucksum += *sptr++;
	ucksum = (ucksum >> 16) + (ucksum & 0xffff);
	ucksum += (ucksum >> 16);

	return (short)(~ucksum & 0xffff);
}
