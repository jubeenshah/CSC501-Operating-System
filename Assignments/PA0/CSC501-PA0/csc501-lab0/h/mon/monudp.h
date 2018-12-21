#ifndef _MONUDP_H_
#define _MONUDP_H_

/* udp.h */

/* User Datagram Protocol (UDP) constants and formats */

#define	U_HLEN	8		/* UDP header length in bytes		*/

/* maximum data in UDP packet	*/
#define	U_MAXLEN	(IP_MAXLEN-(IP_MINHLEN<<2)-U_HLEN)

struct	udp {				/* message format of DARPA UDP	*/
	unsigned short	u_src;		/* source UDP port number	*/
	unsigned short	u_dst;		/* destination UDP port number	*/
	unsigned short	u_len;		/* length of UDP data		*/
	unsigned short	u_cksum;	/* UDP checksum (0 => none)	*/
	char	u_data[U_MAXLEN];	/* data in UDP message		*/
};

#endif
