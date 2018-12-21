#ifndef _MON3COM_H_
#define _MON3COM_H_

/* Information for _3COM(tm) network interface cards 			*/
/* 	- currently only _3C905B or better (full bus master)		*/

#define _3C905_CMD_SELWINDOW		0x0800
#define _3C905_CMD_RXRESET		0x2800
#define _3C905_CMD_TXRESET		0x5800
#define _3C905_CMD_DNSTALL		0x3002
#define _3C905_CMD_DNUNSTALL		0x3003
#define _3C905_CMD_TXENABLE		0x4800
#define _3C905_CMD_RXENABLE		0x2000
#define _3C905_CMD_SETRXFILTER		0x8000
#define _3C905_CMD_UPSTALL		0x3000
#define _3C905_CMD_UPUNSTALL		0x3001
#define _3C905_CMD_ACKINTERRUPT		0x6800
#define _3C905_CMD_SETINDICATION	0x7800
#define _3C905_CMD_SETINTERRUPT		0x7000

#define _3C905_OFF_INTSTATUS		0x0E
#define _3C905_OFF_COMMAND		0x0E
#define _3C905_OFF_TXSTATUS		0x1B
#define _3C905_OFF_DNLISTPTR		0x24
#define _3C905_OFF_UPPKTSTATUS		0x30
#define _3C905_OFF_UPLISTPTR		0x38
#define _3C905_OFF_RXFILTER		0x8		/* window 5 */
#define _3C905_OFF_INTERRUPTENA		0xA		/* window 5 */
#define _3C905_OFF_INDICATIONENA	0xC		/* window 5 */

#define _3C905_FLG_TXINDICATE		0x8000
#define _3C905_FLG_DNCOMPLETE		0x10000
#define _3C905_FLG_LASTFRAG		0x80000000
#define _3C905_FLG_UPERROR		0x4000
#define _3C905_FLG_UPCOMPLETE		0x8000
#define _3C905_FLG_RNDUPDEFEAT		0x10000000
#define _3C905_FLG_UPSTALLED		0x2000
#define _3C905_FLG_RXFINDIVIDUAL	0x1
#define _3C905_FLG_RXFMULTICAST		0x2
#define _3C905_FLG_RXFBROADCAST		0x4
#define _3C905_FLG_RXFALL		0x8		/* promiscuous mode */
#define _3C905_FLG_RXHASHFILTER		0x16
#define _3C905_FLG_INTERRUPT		0x1
#define _3C905_FLG_TXCOMPLINT		0x4
#define _3C905_FLG_UPCOMPLINT		0x400
#define _3C905_FLG_CMDINPROGR		0x1000
#define _3C905_FLG_ERRTXRECLAIM		0x2
#define _3C905_FLG_ERRTXSTATUSOVF	0x4
#define _3C905_FLG_ERRTXMAXCOLL		0x8
#define _3C905_FLG_ERRTXUNDERRUN	0x10
#define _3C905_FLG_ERRTXJABBER		0x20

#define _3C905_MSK_INTERRUPTS		(_3C905_FLG_TXCOMPLINT| \
					_3C905_FLG_UPCOMPLINT)
#define _3C905_MSK_TXERROR		0x3E
#define _3C905_MSK_PKTLENGTH		0x1FFF
#define _3C905_MSK_RXFILTER		0x07

#define _3C905_PCI_IOBASE		0x10
#define _3C905_PCI_MEMBASE		0x14
#define _3C905_PCI_IRQ			0x3C

#define _3C905_STT_RECLAIM		0x1

#define _3CCMD(dev, cmd, arg)		outw(dev->iobase + 0xe, cmd | arg)
#define _3CSEL(dev, win)		outw(dev->iobase + 0xe, 0x800 + win)
#define _3CWIN(dev)			(inw(dev->iobase + 0xe) >> 13)

#define _3C905_TXRING			16
#define _3C905_RXRING			16

#define _3COM_VENDOR_ID			0x10b7
#define _3COM_3C905_DEVICE_ID		0x9200

struct dev_3c905 {
	int	      ifn;
	unsigned long pcidev;
	unsigned long iobase;
	unsigned long membase;
	unsigned char irq;
	unsigned char state;
	Eaddr	      hwa;
	unsigned long tx_begin;
	unsigned long tx_end;
	unsigned long rx_begin;		/* the end pointer are on the NIC */
};

struct eth_pd {
	struct eth_pd*	next;
	unsigned long	status;
	void*		buffer;
	unsigned long	length;
};

/* only one device supported */
extern struct dev_3c905	mon_dev_eth;
extern struct eth_pd*	mon_eth_txring;
extern struct eth_pd*	mon_eth_rxring;

int mon_3c905_ethdemux(struct dev_3c905 *);
int mon_3c905_ethxintr(struct dev_3c905 *);

#endif
