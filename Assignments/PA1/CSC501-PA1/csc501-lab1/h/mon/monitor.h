#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <./mon/moncom.h>
#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moni386.h>

/* for booting an image file from network */

/*
 * states
 */
#define BEGIN_BOOTP	10
#define BOOTP_REQ_SENT	11
#define BEGIN_TFTP	12
#define TFTP_REQ_SENT	13
#define BOOT_DONE	14
#define BOOT_ERROR	15
#define BOOTP_RETX      16
#define TFTP_RETX       17

#define MON_MAX_TRY	3

#define	MON_NETBUFS	32		/* number of network buffers	*/
					/* max network buffer length    */
#define	MON_MAXNETBUF	EP_MAXLEN+sizeof(struct ehx)

/* ARP related stuff */
#define MON_ARP_TSIZE	3 
extern struct   arpentry  mon_arptable[];

/* network related stuff */
struct	netinfo	{			/* info and parms. for network	*/
	int	netpool;		/* network packet buffer pool	*/
};
extern  struct  netinfo mon_Net;

extern int mon_boot_state, mon_boot_try;
extern int mon_tftp_block, mon_tftp_bytes, mon_tftp_retx;
extern struct netif *mon_eth_pni; 	/* pointer to network interface */
extern IPaddr mon_tftp_server;
extern char mon_boot_fname[];
extern int mon_clktime, mon_timeout;	/* for retx purpose */

int mon_cmd(char *);
int mon_ethinit();
void mon_arpinit();
int mon_netinit();
int boot_xinu();
int set_evec(u_int xnum, u_long handler);
int start();
int mon_help();
int mon_blkequ(char *first, char *second, int nbytes);
int mon_netwrite(struct ep *pep, int len);
int mon_bootp_parse(struct bootp_msg *p);
int mon_initq();
int mon_bootp_in(struct udp *pudp);
int mon_arp_in(struct netif *pni, struct ep *pep);
int mon_ip_in(struct netif *pni, struct ep *pep);
int mon_lenq(int q);
int mon_enq(int q, char *elt, int key);
int mon_ethwstrt(struct ethdev *ped);
int mon_udp_in(struct netif *pni, struct ep *pep);
int enable();
int mon_tftp_in(struct udp *pudp);
int mon_arpsend(struct arpentry *pae);
int mon_ethdemux(struct ethdev *ped, u_short iobase);
int mon_newq(int size);
int mon_ethintr();
int mon_ni_in(struct netif *pni, struct ep *pep, int len);
int mon_tftp_ack(int block_no);
int mon_pci_init(void);
int mon_find_pci_device(int deviceID, int vendorID, int index);
int mon_3c905_ethinit ();
int *getmem(unsigned nbytes);
unsigned short mon_ethromread(short iobase, short from);
char * mon_headq(int q);
int mon_bootp_request(int secs);
int mon_tftp_req();
int jumptobootcode();
int mon_pcibios_init(void);
int mon_check_pcibios(void);
char *mon_deq(int q);
int mon_pci_bios_read_config_byte(unsigned long dev, int where, unsigned char *value);
int mon_pci_bios_read_config_word(unsigned long dev, int where, unsigned short *value);
int mon_pci_bios_read_config_dword(unsigned long dev, int where, unsigned long *value);
int mon_pci_bios_write_config_byte(unsigned long dev, int where, unsigned char value);
int mon_pci_bios_write_config_word(unsigned long dev, int where, unsigned short value);
int mon_pci_bios_write_config_dword(unsigned long dev, int where, unsigned long value);
int panic(char *msg);
int mon_ethcmd(unsigned short iobase, unsigned short cmd, unsigned short istatus);

#endif
