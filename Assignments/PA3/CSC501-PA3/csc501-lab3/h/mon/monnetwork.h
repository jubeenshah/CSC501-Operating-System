#ifndef _MONNETWORK_H_
#define _MONNETWORK_H_

#define LOCAL static
#define LITTLE_ENDIAN   0x1234
#define BIG_ENDIAN      0x4321
#define BYTE_ORDER      LITTLE_ENDIAN

/* All includes needed for the network */
#include "./mon/monsystypes.h"
#include "./mon/monconf.h"
#include "./mon/monip.h"
#include "./mon/monether.h"
#include "./mon/monudp.h"
#include "./mon/monbootp.h"
#include "./mon/monarp.h"
#include "./mon/monnetif.h"

/* Declarations data conversion and checksum routines */
extern unsigned short 	mon_cksum();    /* 1s comp of 16-bit 1s comp sum*/

#if	BYTE_ORDER == LITTLE_ENDIAN
#define hs2net(x) (unsigned) ((((x)>>8) &0xff) | (((x) & 0xff)<<8))
#define	net2hs(x) hs2net(x)
#define hl2net(x)       ((((((x)& 0xff)<<24) | (((x)>>24) & 0xff))) | \
        ((((x) & 0xff0000)>>8) | (((x) & 0xff00)<<8)))
#define net2hl(x) hl2net(x)
#endif

#if	BYTE_ORDER == BIG_ENDIAN
#define hs2net(x) (x)
#define net2hs(x) (x)
#define hl2net(x) (x)
#define net2hl(x) (x)
#endif

/* network macros */
#define hi8(x)   (unsigned char)  (((long) (x) >> 16) & 0x00ff)
#define low16(x) (unsigned short) ((long) (x) & 0xffff)

#define	BYTE(x, y)	((x)[(y)]&0xff)	/* get byte "y" from ptr "x" */

#endif
