/* 
 * arp.c - A simple ARP handler
 * 
 * Author:	John C. Lin
 * 		Dept. of Computer Science
 * 		Purdue University
 * Date:	Thu Jun 29 10:19:43 1995
 */

#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <stdio.h>

struct  arpentry        *mon_arpfind(), *mon_arpalloc();
struct	arpentry	mon_arptable[MON_ARP_TSIZE];

/*------------------------------------------------------------------------
 * mon_arpinit  -  initialize data structures for ARP processing
 *------------------------------------------------------------------------
 */
void mon_arpinit()
{
    int	i;

    for (i=0; i<MON_ARP_TSIZE; ++i)
	mon_arptable[i].ae_state = AS_FREE;
}

/*------------------------------------------------------------------------
 *  mon_arp_in  -  handle ARP packet coming in from Ethernet network
 *	N.B. - Called by ni_in-- SHOULD NOT BLOCK
 *------------------------------------------------------------------------
 */
int mon_arp_in(struct netif *pni, struct ep *pep)
{
    struct	arp		*parp = (struct arp *)pep->ep_data;
    struct	arpentry	*pae;
    int		arplen;

    parp->ar_hwtype = net2hs(parp->ar_hwtype);
    parp->ar_prtype = net2hs(parp->ar_prtype);
    parp->ar_op = net2hs(parp->ar_op);

    if (parp->ar_hwtype != AR_HARDWARE || parp->ar_prtype != EPT_IP) {
	freebuf(pep);
	return OK;
    }
    
    if ((pae = mon_arpfind(SPA(parp)))) {
	blkcopy(pae->ae_hwa, SHA(parp), EP_ALEN);
    }
    
    if (!mon_blkequ(TPA(parp), (char *)&pni->ni_ip, IP_ALEN)) {
	freebuf(pep);
	return OK;
    }
    
    if (pae == 0) {
	/*
	 * add a new RESOLVED entry to the cache
	 */
	pae = mon_arpalloc();
	pae->ae_pep = 0;
	blkcopy(pae->ae_hwa, SHA(parp), EP_ALEN);
	blkcopy(pae->ae_pra, SPA(parp), IP_ALEN);
	pae->ae_state = AS_RESOLVED;
    }
    
    if (pae->ae_state == AS_PENDING) {
	pae->ae_state = AS_RESOLVED;
	/*
	 * send the buffered packet
	 */    
	if (pae->ae_pep) {
	    mon_netwrite(pae->ae_pep, pae->ae_pep->ep_len);
	    /* (mon_nif[0].ni_write)(pae->ae_pep, pae->ae_pep->ep_len); */
	    pae->ae_pep = 0;
	}
    }
    
    if (parp->ar_op == AR_REQUEST) {
	parp->ar_op = AR_REPLY;
	blkcopy(TPA(parp), SPA(parp), parp->ar_prlen);
	blkcopy(THA(parp), SHA(parp), parp->ar_hwlen);
	blkcopy(pep->ep_dst, THA(parp), EP_ALEN);
	blkcopy(SHA(parp), pni->ni_hwa.ha_addr, EP_ALEN);
	blkcopy(SPA(parp), &pni->ni_ip, IP_ALEN);

	parp->ar_hwtype = hs2net(parp->ar_hwtype);
	parp->ar_prtype = hs2net(parp->ar_prtype);
	parp->ar_op = hs2net(parp->ar_op);

	arplen = ARP_HLEN + 2*(parp->ar_prlen + parp->ar_hwlen);

	mon_nif[0].ni_write(pep, arplen);
	/* mon_ethwrite(pep, arplen); */
    } else
	freebuf(pep);
    return OK;
}

/*------------------------------------------------------------------------
 * mon_arpalloc - allocate an entry in the ARP table
 * N.B. Assume interrupt disabled
 *------------------------------------------------------------------------
 */
struct arpentry *mon_arpalloc()
{
    int i;
    struct arpentry *pae;
    
    for (i = 0; i < MON_ARP_TSIZE; i++) {
        pae = &mon_arptable[i];
        if (pae->ae_state == AS_FREE)
            break;
    }

    if (i == MON_ARP_TSIZE)
        pae = &mon_arptable[0];

    return(pae);
}

/*------------------------------------------------------------------------
 * mon_arpfind - find an ARP entry given a protocol address and interface
 * N.B. Assume interrupt disabled
 *------------------------------------------------------------------------
 */
struct arpentry *mon_arpfind(char *pra)
{
    struct arpentry	*pae;
    int		i;

    for (i=0; i<MON_ARP_TSIZE; ++i) {
	pae = &mon_arptable[i];
	if (pae->ae_state == AS_FREE)
	    continue;
	
	if (mon_blkequ(pae->ae_pra, pra, IP_ALEN))
	    return pae;
    }
    return 0;
}

/*------------------------------------------------------------------------
 * mon_arpsend - broadcast an ARP request
 *	N.B. Assumes interrupts disabled
 *------------------------------------------------------------------------
 */
int mon_arpsend(struct arpentry *pae)
{
    struct	netif	*pni = &mon_nif[0];
    struct	ep	*pep;
    struct	arp	*parp;
    int		arplen;

    pep = (struct ep *)getbuf(mon_bufpool);
    if ((int)pep == SYSERR)
	return SYSERR;
    
    blkcopy(pep->ep_dst, pni->ni_hwb.ha_addr, EP_ALEN);
    pep->ep_type = EPT_ARP;
    parp = (struct arp *) pep->ep_data;
    parp->ar_hwtype = hs2net(AR_HARDWARE);
    parp->ar_prtype = hs2net(EPT_IP);
    parp->ar_hwlen = EP_ALEN; 
    parp->ar_prlen = IP_ALEN;
    parp->ar_op = hs2net(AR_REQUEST);
    blkcopy(SHA(parp), pni->ni_hwa.ha_addr, EP_ALEN);
    blkcopy(SPA(parp), &pni->ni_ip, IP_ALEN);
    bzero(THA(parp), EP_ALEN);
    blkcopy(TPA(parp), pae->ae_pra, IP_ALEN);
    arplen = ARP_HLEN + 2*(parp->ar_hwlen + parp->ar_prlen);
    (mon_nif[0].ni_write)(pep, EP_HLEN+arplen);
    /* mon_ethwrite(pep, EP_HLEN+arplen); */
    return OK;
}
