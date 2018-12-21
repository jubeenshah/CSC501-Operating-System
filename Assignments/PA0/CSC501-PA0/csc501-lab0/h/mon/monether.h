#ifndef _MONETHER_H_
#define _MONETHER_H_

/* ether.h */

/* Ethernet definitions and constants */

#define EP_MAXMULTI       10    /* multicast address table size         */

#define	EP_MINLEN	  60	/* minimum packet length		*/
#define	EP_DLEN		1500	/* length of data field ep		*/
#define	EP_HLEN		  14	/* size of (extended) Ether header	*/
#define	EP_ALEN		   6	/* number of octets in physical address	*/
#define	EP_CRC		   4	/* ether CRC (trailer)			*/
#define	EP_MAXLEN	(EP_HLEN+EP_DLEN)
typedef	unsigned char	Eaddr[EP_ALEN]; /* a physical Ethernet address	*/
#define	EP_RETRY	3	/* number of times to retry xmit errors	*/
#define	EP_BRC	"\377\377\377\377\377\377"/* Ethernet broadcast address	*/
#define EP_RTO 300		/* time out in seconds for reads	*/

#define EP_NUMRCV 16		/* number LANCE recv buffers (power 2)	*/
#define EP_NUMRCVL2 4		/* log2 of the number of buffers	*/

#define	EPT_LOOP	0x0060		/* type: Loopback		*/
#define	EPT_ECHO	0x0200		/* type: Echo			*/
#define	EPT_PUP		0x0400		/* type: Xerox PUP		*/
#define	EPT_IP		0x0800		/* type: Internet Protocol	*/
#define	EPT_ARP		0x0806		/* type: ARP			*/
#define	EPT_RARP	0x8035		/* type: Reverse ARP		*/

struct	eh {			/* ethernet header			*/
	Eaddr	eh_dst;		/* destination host address		*/
	Eaddr	eh_src;		/* source host address			*/
	unsigned short	eh_type;/* Ethernet packet type (see below)	*/
};

struct  ehx {                   /* Xinu extended header                 */
	IPaddr  ehx_nexthop;    /* nexthop IP address                   */
	short   ehx_ifn;        /* originating interface number         */
	short   ehx_len;        /* length of the packet                 */
};

struct  ep      {               /* complete structure of Ethernet packet*/
	struct  ehx ep_ehx;     /* Xinu extended header                 */
	struct  eh ep_eh;       /* the ethernet header                  */
	char    ep_data[EP_DLEN];       /* data in the packet           */
};

/* these allow us to pretend it's all one big happy structure */
#define ep_nexthop	ep_ehx.ehx_nexthop
#define ep_ifn		ep_ehx.ehx_ifn
#define ep_len		ep_ehx.ehx_len
#define	ep_dst		ep_eh.eh_dst
#define	ep_src		ep_eh.eh_src
#define	ep_type		ep_eh.eh_type

#define	ETOUTQSZ	16
#define	ETINQSZ		16

/* ethernet function codes */

#define	EPC_PROMON	1		/* turn on promiscuous mode	*/
#define	EPC_PROMOFF	2		/* turn off promiscuous mode	*/
#define	EPC_MADD	3		/* add multicast address	*/
#define	EPC_MDEL	4		/* delete multicast address	*/

struct etdev_comm {
    Eaddr		 edc_paddr;	/* hardware address		*/
    Eaddr		 edc_bcast;
    int		 	 edc_outq;
};
#define ed_paddr	eddev_comm.edc_paddr
#define ed_bcast	eddev_comm.edc_bcast
#define ed_outq		eddev_comm.edc_outq

/*
 * The structure is for EtherExpress PRO/10 port I/O
 */
struct ethdev {
    struct etdev_comm   eddev_comm;     /* common part                  */
    unsigned short      ed_iobase;
    unsigned short      ed_irq;
    unsigned short      ed_rx_start;    /* start of the RX chain        */
    unsigned short      ed_tx_start;    /* start of the TX chain        */
    unsigned short      ed_tx_end;      /* end of the TX chain          */
    unsigned short      ed_tx_chain_bit;/* position to set chain bit	*/
    unsigned short      ed_tx_chain_cnt;/* chain count                  */
    unsigned long       ed_mem_size;    /* on-board memory size         */
    unsigned short      ed_txbuf_size;  /* TX buffer size               */
    unsigned short      ed_rxbuf_size;  /* RX buffer size               */
};

extern struct ethdev     mon_eth[];

#endif
