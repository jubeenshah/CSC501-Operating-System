#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/montftp.h>
#include <./mon/monbootp.h>
#include <stdio.h>

int mon_tftp_block;		/* tftp block count */
int mon_tftp_bytes;		/* tftp byte count */
int mon_tftp_retx;              /* TFTP ACK retx count */
short mon_tftp_server_port;
IPaddr mon_tftp_server;
char *mon_tftp_memloc;

#define CONTINUE	99

int mon_tftp_ack(int block_no);
int mon_netwrite(struct ep *pep, int len);

/*
#define PRINTERR
#define VERBOSE
#define DEBUG
*/

/*-------------------------------------------------------------------------
 * mon_tftp_in - 
 *-------------------------------------------------------------------------
 */
int mon_tftp_in(struct udp *pudp)
{
    struct tftp_data *rd;
    int nbytes, diff;

    mon_tftp_server_port = pudp->u_src;
    
    rd = (struct tftp_data *) pudp->u_data;
    rd->type = net2hs(rd->type);
    rd->block = net2hs(rd->block);

    if (rd->type == TFTP_TYPE_ERROR) {
#ifdef PRINTERR
	kprintf("TFTP: Received error (code = %d)\n", rd->block);
#endif
	return SYSERR;
    }
    
    if (rd->block != mon_tftp_block) {
#ifdef PRINTERR
	kprintf("TFTP: expecting block %d, got %d\n", mon_tftp_block,
		rd->block);
#endif
	diff = mon_tftp_block - rd->block;
	if (diff == 1)
		mon_tftp_ack(rd->block);	/* lost ACK? */
	else if  (diff == -1)
		mon_tftp_ack(mon_tftp_block); 	/* lost DATA? */
	return (CONTINUE);
    }

    /*
     * copy to memory, then ACK
     */
    nbytes = pudp->u_len - TFTP_DATA_HDR_SZ - U_HLEN;
    blkcopy(mon_tftp_memloc, rd->data, nbytes);
    mon_tftp_memloc += nbytes;
    mon_tftp_bytes += nbytes;

    /*
     * ACK it
     */
    mon_tftp_ack(mon_tftp_block);

    if ((mon_tftp_block % 2) == 0)
	kprintf(".");

    if (nbytes < TFTP_BLOCK_SIZE) {
	kprintf("\ntotal = %d octets.\n", mon_tftp_bytes);
	return OK;
    }
    
    mon_tftp_block++;

    return (CONTINUE);
}	

/*-------------------------------------------------------------------------
 * mon_tftp_req - send a TFTP request
 *-------------------------------------------------------------------------
 */
int mon_tftp_req()
{
    struct tftp_req req;
    struct ep *pep;
    struct ip *pip;
    struct udp *pup;
    int len, slen;

    pep = (struct ep *)getbuf(mon_bufpool);
    if (pep == 0) {
#ifdef PRINTERR
	kprintf("tftp_req: !! no buffer\n");
#endif
	return SYSERR;
    }
    pip = (struct ip *)pep->ep_data;
    pup = (struct udp *)pip->ip_data;

    /*
     * send a Read Request to server
     */
    slen = strlen(mon_boot_fname);
    bzero(&req, sizeof(struct tftp_req));
    req.type = hs2net(TCTP_TYPE_RRQ);	 
    strcpy(req.data, mon_boot_fname);
    strcpy(&req.data[slen + 1], "octet");
    len = sizeof(req.type) + slen + 1 + 5 + 1;
#ifdef DEBUG
    kprintf("mon_tftp_req: file name = [%s], len=%d, udp len=%d\n",
	    mon_boot_fname, slen, len);
#endif

    /* set TFTP request data */
    blkcopy(pup->u_data, (char *)&req, len);
    
    /* set UDP header info */
    pup->u_src = hs2net(TFTP_MY_TID);
    pup->u_dst = hs2net(TFTP_INIT_TID);
    pup->u_len = hs2net(U_HLEN + len);
    pup->u_cksum = 0;

    /* set IP header */
    pip->ip_proto = IPT_UDP;
    pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
    pip->ip_tos = 0;
    pip->ip_len = hs2net(IPMHLEN + U_HLEN + len);
    pip->ip_id = 0;
    pip->ip_fragoff = 0;
    pip->ip_ttl = IP_TTL;
    pip->ip_src = mon_eth_pni->ni_ip;
    pip->ip_dst = mon_tftp_server;
    pip->ip_cksum = 0;
    pip->ip_cksum = mon_cksum(pip, IPMHLEN);

    /* now set the ethernet info */
    pep->ep_nexthop = mon_tftp_server;
    pep->ep_eh.eh_type = EPT_IP;

    mon_tftp_memloc = (char *)BOOTPLOC;
    mon_tftp_block = mon_tftp_bytes = 0;

    /* use ARP to get server's hardware address */
    /* (mon_nif[0].ni_write)(pep, EP_HLEN+net2hs(pip->ip_len)); */
    mon_netwrite(pep, EP_HLEN+net2hs(pip->ip_len));
    return(OK);
}

/*-------------------------------------------------------------------------
 * mon_tftp_ack - ACK a TFTP data packet
 *-------------------------------------------------------------------------
 */
int mon_tftp_ack(int block_no)
{
    struct tftp_ack *p;
    struct ep *pep;
    struct ip *pip;
    struct udp *pup;

    pep = (struct ep *)getbuf(mon_bufpool);
    if (pep == 0) {
#ifdef PRINTERR
	kprintf("tftp_ack: !! no buffer\n");
#endif
	return SYSERR;
    }
    pip = (struct ip *)pep->ep_data;
    pup = (struct udp *)pip->ip_data;

    /* set UDP header info */
    pup->u_src = hs2net(TFTP_MY_TID);
    pup->u_dst = hs2net(mon_tftp_server_port);
    pup->u_len = hs2net(U_HLEN + TFTP_ACK_SZ);
    pup->u_cksum = 0;

    /* UDP data part */
    p = (struct tftp_ack *)pup->u_data;
    p->type = hs2net(TFTP_TYPE_ACK);
    p->block = hs2net(block_no);

    /* set IP header */
    bzero(pip, IPMHLEN);
    pip->ip_proto = IPT_UDP;
    pip->ip_verlen = (IP_VERSION<<4) | IP_MINHLEN;
    pip->ip_len = hs2net(IPMHLEN + U_HLEN + TFTP_ACK_SZ);
    pip->ip_ttl = 5;
    pip->ip_src = mon_eth_pni->ni_ip;
    pip->ip_dst = mon_tftp_server;
    pip->ip_cksum = mon_cksum(pip, IPMHLEN);

    /* now set the ethernet info */
    pep->ep_nexthop = mon_tftp_server;
    pep->ep_eh.eh_type = EPT_IP;
    
    mon_timeout = mon_clktime + 2;    /* start retx timer */

    /* use ARP to get server's hardware address */
    /* return((mon_nif[0].ni_write)(pep, EP_HLEN+U_HLEN+IPMHLEN+TFTP_ACK_SZ)); */
    return(mon_netwrite(pep, EP_HLEN+U_HLEN+IPMHLEN+TFTP_ACK_SZ)); 
}

