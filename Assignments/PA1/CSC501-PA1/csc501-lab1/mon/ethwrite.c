#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <stdio.h>

/*
#define DEBUG
#define PRINTERRORS
*/

/*------------------------------------------------------------------------
 * mon_ethwrite - write a single packet to an Intel EtherExpress PRO/10
 *------------------------------------------------------------------------
 */
int mon_ethwrite(pep, len)
struct ep       *pep;
int             len;
{
    struct ethdev	*ped;
    STATWORD            ps;

#ifdef DEBUG
    kprintf("eep_write\n");
#endif

    ped = &mon_eth[0];

    if (len > EP_MAXLEN) {
#ifdef PRINTERRORS
        kprintf("eep_write: len(%d) > EP_MAXLEN(%d)\n", len, EP_MAXLEN);
#endif
        freebuf(pep);
        return SYSERR;
    }

    if (len < EP_MINLEN)
        len = EP_MINLEN;

    blkcopy(pep->ep_src, ped->ed_paddr, EP_ALEN);
    pep->ep_len = len;
    pep->ep_type = hs2net(pep->ep_type);

#ifdef PRINTERRORS
        kprintf("eep_write: enq len = %d\n", len);
#endif

    if (mon_enq(ped->ed_outq, (char *) pep, 0) < 0) {
        kprintf("eep_write: qull full (len=%d)\n", mon_lenq(ped->ed_outq));
        freebuf(pep);
    }
    
    disable(ps);
    mon_ethwstrt(ped);
    restore(ps);
    return OK;
}
