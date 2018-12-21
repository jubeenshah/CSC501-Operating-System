/* 
 * bootp.c - BOOTP related modules 
 */
#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/monbootp.h>
#include <stdio.h>

static int make_bootp_packet(struct bootp_msg *bptr, int secs);
char mon_boot_fname[80];

/*
#define DEBUG
#define PRINTERR
 */

/*-------------------------------------------------------------------------
 * mon_bootp_in - 
 *-------------------------------------------------------------------------
 */
int mon_bootp_in(struct udp *pudp)
{
    struct bootp_msg *p;
    
#ifdef DEBUG
        kprintf("mon_bootp_in\n");
#endif

    p = (struct bootp_msg *)pudp->u_data;
    if (p->op == BOOTREPLY) {
#ifdef DEBUG
	kprintf("got a BOOTP reply\n");
#endif
	if (mon_bootp_parse(p) != OK) {
#ifdef PRINTERR
	    kprintf("BAD BOOTP Reply\n");
#endif
	    return SYSERR;
	}
    }

    return OK;
}

/*-------------------------------------------------------------------------
 * mon_bootp_parse -
 *-------------------------------------------------------------------------
 */
int mon_bootp_parse(struct bootp_msg *p)
{
    
    unsigned long cookie;

    cookie = *(unsigned long *)p->vend;
    if (net2hl(cookie) != RFC1084) {
#ifdef PRINTERR
        kprintf("Incorrect RFC 1084 magic cookie in BOOTP reply.\n");
#endif
        return SYSERR;
    }

    /* grab my IP addr */
    mon_eth_pni->ni_ip = p->yiaddr;
    
    /* boot server IP */
    mon_tftp_server = p->siaddr;
    
    /* boot file name */
    strcpy(mon_boot_fname, p->file);

#ifdef DEBUG
    kprintf("my IP addr is %x.\n", net2hl(mon_eth_pni->ni_ip));
    kprintf("Boot file: %s, server: %x.\n", mon_boot_fname,
	    net2hl(mon_tftp_server));
#endif

    return(OK);
}

/*-------------------------------------------------------------------------
 * mon_bootp_request -
 *-------------------------------------------------------------------------
 */
int mon_bootp_request(int secs)
{
    struct bootp_msg bppacket;
    struct ep *pep;
    struct ip *pip;
    struct udp *pup;

    pep = (struct ep *)getbuf(mon_bufpool);
    if (pep == 0) {
#ifdef PRINTERR
	kprintf("bootp_request: !! no buffer\n");
#endif
	return SYSERR;
    }
    
    pip = (struct ip *)pep->ep_data;
    pup = (struct udp *)pip->ip_data;
   
    make_bootp_packet(&bppacket, secs);

    /* set the BOOTP data */
    blkcopy(pup->u_data, &bppacket, BOOTP_SIZE);

    /* now set the UDP header info */
    pup->u_src = hs2net(BOOTP_CPORT);
    pup->u_dst = hs2net(BOOTP_SPORT);
    pup->u_len = hs2net(U_HLEN + BOOTP_SIZE);
    pup->u_cksum = 0;

    
    /* now set the IP header info */
    pip->ip_proto = IPT_UDP;
    pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
    pip->ip_tos = 0;
    pip->ip_len = hs2net(IPMHLEN + U_HLEN + BOOTP_SIZE);
    pip->ip_id = 0;
    pip->ip_fragoff = 0;
    pip->ip_ttl = IP_TTL;
    pip->ip_src = 0;
    pip->ip_dst = mon_ip_maskall;
    pip->ip_cksum = 0;
    pip->ip_cksum = mon_cksum(pip, IPMHLEN);

    /* now set the ethernet info */
    blkcopy(pep->ep_eh.eh_dst, EP_BRC, EP_ALEN);
    pep->ep_eh.eh_type = EPT_IP;
    return((mon_nif[0].ni_write)(pep, EP_HLEN+U_HLEN+IPMHLEN+BOOTP_SIZE));
}

/*-------------------------------------------------------------------------
 * make_bootp_packet -
 *-------------------------------------------------------------------------
 */
static int make_bootp_packet(struct bootp_msg *bptr, int secs)
{
    bzero(bptr, BOOTP_SIZE);
    bptr->op = BOOTREQUEST;
    bptr->htype = AR_HARDWARE;
    bptr->hlen = EP_ALEN;
    bptr->xid = 47;             /* just a random number that's nonzero */
    bptr->secs = hs2net(secs);
    blkcopy(bptr->chaddr, (char *)&(mon_eth[0].ed_paddr), EP_ALEN);
    *(int *)bptr->vend = hl2net(RFC1084);
    bptr->vend[5] = 0xff;
#ifdef DEBUG
    kprintf("Using ethernet address %02x:%02x:%02x:%02x:%02x:%02x\n",
            bptr->chaddr[0], bptr->chaddr[1], bptr->chaddr[2],
            bptr->chaddr[3], bptr->chaddr[4], bptr->chaddr[5]);
#endif
    return(OK);
}


