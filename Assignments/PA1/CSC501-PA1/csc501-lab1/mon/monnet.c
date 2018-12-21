#include <./mon/monnetwork.h>
#include <./mon/moni386.h>
#include <./mon/monitor.h>
#include <bufpool.h>
#include <stdio.h>

int		mon_bufpool;
struct	netif	mon_nif[1];
IPaddr		mon_ip_maskall = -1;
IPaddr		mon_ip_anyaddr = 0;

/*#define	DEBUG*/

/*------------------------------------------------------------------------
 * mon_init - initialize monitor related stuff
 *------------------------------------------------------------------------
 */
int mon_init()
{
    mon_initq();
    mon_bufpool = mkpool(1530, 16);
    return(OK);
}

/*------------------------------------------------------------------------
 * mon_netinit - init nif[] 
 *------------------------------------------------------------------------
 */
int mon_netinit()
{
    struct ethdev *ped = &mon_eth[0];
    extern short girmask;
    extern int   mon_clkint();	/* clock int. handler, for retx purpose */
    
    blkcopy(mon_nif[0].ni_hwa.ha_addr, ped->ed_paddr, EP_ALEN);
    blkcopy(mon_nif[0].ni_hwb.ha_addr, ped->ed_bcast, EP_ALEN);
    mon_nif[0].ni_state = NIS_UP;

    girmask = 0xfffb;
    /*
     * 1. Set monitor's clock interrupt handler (for retx purpose).
     *    (It sets the corresponding mask bit in "girmask" to 0.)
     * 2. Sets the Ethernet mask bit in "girmask" to 0.
     * 3. call enable() to enable clock & ethernet interrupts.
     */
    set_evec(IRQBASE, (unsigned int) mon_clkint);
    girmask = (girmask & ~(1 << ped->ed_irq));
    enable();
    return(OK);
}

/*------------------------------------------------------------------------
 *  mon_ni_in - network interface input function
 *------------------------------------------------------------------------
 */
int mon_ni_in(pni, pep, len)
struct	netif	*pni;		/* the interface	*/
struct	ep	*pep;		/* the packet		*/
int		len;		/* length, in octets	*/
{
    switch (pep->ep_type) {
    case EPT_ARP:
#ifdef DEBUG
	kprintf("mon_ni_in: Got ARP\n");
#endif
	mon_arp_in(pni, pep);
	break;
	
    case EPT_IP:
#ifdef DEBUG
	kprintf("mon_ni_in: Got IP\n");
#endif
	mon_ip_in(pni, pep);
	break;
	
    default:
	freebuf(pep);
	return OK;
    }
    return(OK);
}

struct	arpentry *mon_arpfind(), *mon_arpalloc();

/*------------------------------------------------------------------------
 * mon_netwrite - write a packet on an interface, using ARP if needed
 *------------------------------------------------------------------------
 */
int mon_netwrite(pep, len)
struct	ep	*pep;
int		len;
{
    struct	netif	*pni = &mon_nif[0];
    struct	arpentry 	*pae;
    STATWORD	ps;

    if (pni->ni_state != NIS_UP) {
	freebuf(pep);
	return SYSERR;
    }
    
    pep->ep_len = len;

    if (pep->ep_nexthop == mon_ip_anyaddr||pep->ep_nexthop == mon_ip_maskall){
	blkcopy(pep->ep_dst, pni->ni_hwb.ha_addr, EP_ALEN);
	(mon_nif[0].ni_write)(pep, len);
	/* mon_ethwrite(pep, len); */
	return OK;
    }

    /* else, look up the protocol address... */

    disable(ps);

    pae = mon_arpfind(&pep->ep_nexthop);
    
    if (pae) {
	if (pae->ae_state == AS_RESOLVED) {
	    blkcopy(pep->ep_dst, pae->ae_hwa, EP_ALEN);
	    restore(ps);
	    /* return (mon_ethwrite(pep, len)); */
	    return ((mon_nif[0].ni_write)(pep, len));
	}
	else if (pae->ae_state == AS_PENDING) {
	    if (pae->ae_pep)
		freebuf(pae->ae_pep);
	    pae->ae_pep = pep;
	    mon_arpsend(pae);
	    restore(ps);
	    return(OK);
	}
    }
 
    if (IP_CLASSD(pep->ep_nexthop)) {
	restore(ps);
	return SYSERR;
    }

    if (pae == 0) {
	pae = mon_arpalloc();
	pae->ae_state = AS_PENDING;
	pae->ae_pep = pep;
	blkcopy(pae->ae_pra, &pep->ep_nexthop, IP_ALEN);
	mon_arpsend(pae);
    }
    
    restore(ps);
    return OK;
}

/*------------------------------------------------------------------------
 * mon_blkequ  -  return TRUE iff one block of memory is equal to another
 *------------------------------------------------------------------------
 */
int mon_blkequ(char *first, char *second, int nbytes)
{
    while (--nbytes >= 0)
	if (*first++ != *second++)
	    return(FALSE);
    return(TRUE);
}

