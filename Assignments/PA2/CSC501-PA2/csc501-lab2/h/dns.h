#ifndef _DNS_H
#define _DNS_H

#define HOSTNAMELEN	32

#define DN_QTHA         1
#define DN_QCIN         1

#define DNS_SERVER	"128.211.1.10:53"
#define DNS_SUFFIX	".cs.purdue.edu"
#define DN_NTRIES          5

struct dns {
	short dn_id;
	short dn_opparm;
	short dn_qcount;
	short dn_acount;
	short dn_ncount;
	short dn_dcount;
	char  dn_data[1];
};

IPaddr hosttoip (char *host);	/* Return IPADDR_ANY on error */
int isdottedquad (char *addr);
int gethostbyname (IPaddr *addr, char *host);
int cspecparse (char *spec, IPaddr *addr, u_short *port, int *active);

#endif
