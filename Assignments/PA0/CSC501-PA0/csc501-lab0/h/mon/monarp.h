#ifndef _MONARP_H_
#define _MONARP_H_

/* arp.h - SHA, SPA, THA, TPA */

/* Internet Address Resolution Protocol  (see RFCs 826, 920)		*/

#define	AR_HARDWARE	1	/* Ethernet hardware type code		*/

/* Definitions of codes used in operation field of ARP packet */
	
#define	AR_REQUEST	1	/* ARP request to resolve address	*/
#define	AR_REPLY	2	/* reply to a resolve request		*/

#define	RA_REQUEST	3	/* reverse ARP request (RARP packets)	*/
#define	RA_REPLY	4	/* reply to a reverse request (RARP ")	*/

struct	arp	{
	short	ar_hwtype;	/* hardware type			*/
	short	ar_prtype;	/* protocol type			*/
	char	ar_hwlen;	/* hardware address length		*/
	char	ar_prlen;	/* protocol address length		*/
	short	ar_op;		/* ARP operation (see list above)	*/
	char	ar_addrs[1];	/* sender and target hw & proto addrs	*/
};

#define	SHA(p)	(&p->ar_addrs[0])
#define	SPA(p)	(&p->ar_addrs[(int) p->ar_hwlen])
#define	THA(p)	(&p->ar_addrs[p->ar_hwlen + p->ar_prlen])
#define	TPA(p)	(&p->ar_addrs[(p->ar_hwlen*2) + p->ar_prlen])


#define	MAXHWALEN	EP_ALEN	/* Ethernet				*/
#define	MAXPRALEN	IP_ALEN	/* IP					*/

#define ARP_HLEN	8	/* ARP header length			*/

struct	arpentry {		/* format of entry in ARP cache		*/
	short	ae_state;	/* state of this entry (see below)	*/
	char	ae_hwa[MAXHWALEN];	/* Hardware address 		*/
	char	ae_pra[MAXPRALEN];	/* Protocol address 		*/
    	struct ep *ae_pep;	/* packet waiting			*/
};

#define	AS_FREE		0	/* Entry is unused (initial value)	*/
#define	AS_PENDING	1	/* Entry is used but incomplete		*/
#define	AS_RESOLVED	2	/* Entry has been resolved		*/

#endif
