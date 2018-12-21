#ifndef _PING_H_
#define _PING_H_

/* Default arguments */
#define PINGD_COUNT	10
#define PINGD_TTL	255
#define PINGD_PATTERN	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define PINGD_PLEN	0
#define PINGD_SIZE	56
#define PINGD_ID	0
#define PINGD_DST	IPADDR_ANY
#define PINGD_INT	1000
#define PINGD_FLOOD	FALSE

#define PINGMAX_PLEN	16

#define PING_TIMEOUT	1000

struct ping_args {
	u_int arg_count;
	u_char arg_ttl;
	u_char arg_pattern[PINGMAX_PLEN];
	int arg_plen;
	u_short arg_size;
	u_short arg_id;
	IPaddr arg_dst;
	int arg_interval;
	int arg_flood;
};

#endif	/* _PING_H_ */
