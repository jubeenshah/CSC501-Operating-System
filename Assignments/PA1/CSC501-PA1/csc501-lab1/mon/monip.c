#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <stdio.h>

/*#define PRINTERR*/
/*#define DEBUG*/

/*------------------------------------------------------------------------
 * mon_ip_in - 
 *------------------------------------------------------------------------
 */
int mon_ip_in(struct netif *pni, struct ep *pep)
{
    struct  ip      *pip;

    pip = (struct ip *)pep->ep_data;
    
#ifdef DEBUG
    kprintf("mon_ip_in: src=%x, dst=%x, proto=%d, len=%d\n", pip->ip_src,
	    pip->ip_dst, pip->ip_proto, net2hs(pip->ip_len));
#endif
    
    if ((pip->ip_verlen>>4) != IP_VERSION) {
#ifdef PRINTERR
	kprintf("ip_in: !! bad version\n");
#endif
	freebuf(pep);
	return(OK);
    }
    
    if (IP_CLASSE(pip->ip_dst)) {
#ifdef PRINTERR
	kprintf("mon_ip_in: class E IP??\n");
#endif
	freebuf(pep);
	return(OK);
    }
    
    if (mon_cksum(pip, IP_HLEN(pip))) {
#ifdef PRINTERR
	kprintf("ip_in: !! bad checksum\n");
#endif
	freebuf(pep);
	return(OK);
    }

    /*
     * convert to host byte order
     */
    pip->ip_len = net2hs(pip->ip_len);
    pip->ip_id = net2hs(pip->ip_id);

    switch (pip->ip_proto) {
    case IPT_UDP:
#ifdef DEBUG
	kprintf("mon_ip_in: UDP in\n");
#endif
	mon_udp_in(pni, pep);
	break;
	
    default:
#ifdef DEBUG
	kprintf("mon_ip_in: Not UDP, proto tyep = %d\n", pip->ip_proto);
#endif
	freebuf(pep);
	return(OK);
    }
    return(OK);
}

