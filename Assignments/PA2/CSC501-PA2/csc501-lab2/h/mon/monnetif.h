#ifndef _MONNETIF_H_
#define _MONNETIF_H_

#define	NI_MAXHWA EP_ALEN		/* max size of any hardware	*/
					/*  (physical) net address	*/
struct	hwa	{			/* a hardware address		*/
	unsigned char	ha_addr[NI_MAXHWA]; 	/* the address		*/
};

/* interface states */
#define NIS_UP          0x01
#define NIS_DOWN        0x02
#define NIS_TESTING     0x03

/* Definitions of network interface structure (one per interface)	*/

struct	netif {				/* info about one net interface	*/
	char		ni_state;	/* interface states: NIS_ above	*/
	IPaddr		ni_ip;		/* IP address for this interface*/
	struct	hwa	ni_hwa;		/* hardware address of interface*/
	struct	hwa	ni_hwb;		/* hardware broadcast address	*/
	int             (*ni_write)();  /* write function call pointer  */
};

extern struct netif	mon_nif[];
extern	int mon_bufpool;

#endif
