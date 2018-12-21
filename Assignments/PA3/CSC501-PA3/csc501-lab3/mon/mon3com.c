#if 0
#include <kernel.h>
#include <conf.h>
#endif
#include <mon/moni386.h>
#include <mon/monnetwork.h>
#include <mon/mon3com.h>
#include <mon/monpci.h>
#include <mon/monitor.h>
#include <stdio.h>
/* 
#include <mon/monnetif.h>
*/

/*#define DEBUG*/
#define MAX_RCV			10		/* maximum packets per int */
#define GETBUF			getbuf		/* non-blocking getbuf     */

extern int mon_ethint_hi();

int mon_3c905_ethwrite(struct ep*, int);

struct dev_3c905 mon_dev_eth;
struct eth_pd* mon_eth_txring;
struct eth_pd* mon_eth_rxring;
struct eth_pd  txring[_3C905_TXRING + 1];
struct eth_pd  rxring[_3C905_RXRING + 1];

int ethcmdwait(struct dev_3c905 *dev, unsigned short cmd, unsigned short arg)
{
    int i;

    _3CCMD(dev, cmd, arg);
#ifdef BOOT_MONITOR
    for (i = 0; i < 1 << 30; i++)
#else
    for (i = 0; i < 10000; i++)
#endif
        if (! (inw(dev->iobase + _3C905_OFF_INTSTATUS) & 
               _3C905_FLG_CMDINPROGR))
            return OK;

    /* Note:  This will not work for EEPROM commands 		*/
    /* it should be fixed for a final version using sleep ?	*/

    return SYSERR;
}

int mon_3c905_ethinit () 
{
    struct dev_3c905* dev;
    struct netif*     pni;
    struct ethdev*    ped;
    unsigned short    status;
    unsigned char*    buf;
    int i;

    dev = &mon_dev_eth;
    dev->state = 0;

    /* read config information */
    mon_pci_bios_read_config_dword(dev->pcidev, _3C905_PCI_IOBASE,  &dev->iobase);
    mon_pci_bios_read_config_dword(dev->pcidev, _3C905_PCI_MEMBASE, &dev->membase);
    mon_pci_bios_read_config_byte (dev->pcidev, _3C905_PCI_IRQ,     &dev->irq);

    /* the low bit is set to indicate I/O */
    dev->iobase &= ~1;

    /* if mem address is below 1 MB */
    dev->membase &= ~2;

    /* enable PCI bus master */
    mon_pci_bios_read_config_word(dev->pcidev, PCI_COMMAND, &status);
    status |= PCI_BUSMASTER;
    mon_pci_bios_write_config_word(dev->pcidev, PCI_COMMAND, status);

    /* read the MAC address */
    _3CSEL(dev, 2);
    *((unsigned short*) &dev->hwa[0]) = inw(dev->iobase + 0x00);
    *((unsigned short*) &dev->hwa[2]) = inw(dev->iobase + 0x02);
    *((unsigned short*) &dev->hwa[4]) = inw(dev->iobase + 0x04);

    /* enable this to "emulate" xinu109
    *((unsigned short*) &dev->hwa[0]) = 0xAA00;
    *((unsigned short*) &dev->hwa[2]) = 0xA300;
    *((unsigned short*) &dev->hwa[4]) = 0x8A75;
    outw(dev->iobase + 0x00, *((unsigned short*) &dev->hwa[0]));
    outw(dev->iobase + 0x02, *((unsigned short*) &dev->hwa[2]));
    outw(dev->iobase + 0x04, *((unsigned short*) &dev->hwa[4]));
    */

#ifdef BOOT_MONITOR
    *((unsigned short*) &dev->hwa[0]) |= 0xFF01;
#endif

    /* initialize TX/RX rings */
    mon_eth_txring = (struct eth_pd*) (((unsigned long) txring + 0xF) & ~0xF);
    mon_eth_rxring = (struct eth_pd*) (((unsigned long) rxring + 0xF) & ~0xF);
    for (i = 0; i < _3C905_RXRING; i++) {
        mon_eth_rxring[i].next   = &mon_eth_rxring[(i + 1) % _3C905_RXRING];
        mon_eth_rxring[i].status = 0;
        buf = (unsigned char*) GETBUF(mon_Net.netpool); 
        if (buf == (unsigned char *)SYSERR || buf == (unsigned char *)0) {
            kprintf("No buffer in ethinit()");
            panic("could not allocate buffer for monitor");
        }
        mon_eth_rxring[i].buffer = (void*) buf + sizeof(struct ehx);
        mon_eth_rxring[i].length = _3C905_FLG_LASTFRAG | EP_MAXLEN;
    }
    dev->rx_begin = 0;
    bzero(mon_eth_txring, sizeof(struct eth_pd) * _3C905_TXRING);
    dev->tx_begin = 0;
    dev->tx_end   = _3C905_TXRING - 1;

    /* reset nic */
    ethcmdwait(dev, _3C905_CMD_RXRESET, 0x7);
    ethcmdwait(dev, _3C905_CMD_TXRESET, 0x3);

    /* enable interrupts */
    set_evec(dev->irq + IRQBASE, (unsigned) mon_ethint_hi);
    _3CCMD(dev, _3C905_CMD_SETINDICATION, _3C905_MSK_INTERRUPTS);
    _3CCMD(dev, _3C905_CMD_SETINTERRUPT, _3C905_MSK_INTERRUPTS);

    /* enable receiving and transmission */
    _3CCMD(dev, _3C905_CMD_SETRXFILTER, _3C905_MSK_RXFILTER);
    ethcmdwait(dev, _3C905_CMD_UPSTALL, 0);
    outl(dev->iobase + _3C905_OFF_UPLISTPTR, (int) mon_eth_rxring);
    _3CCMD(dev, _3C905_CMD_UPUNSTALL, 0);
    _3CCMD(dev, _3C905_CMD_RXENABLE, 0);
    _3CCMD(dev, _3C905_CMD_TXENABLE, 0);

    /* set the OS structures */
    dev->ifn = 0;
    pni = &mon_nif[0];
    pni->ni_write = mon_3c905_ethwrite;
    ped = &mon_eth[0];
    for (i = 0; i < EP_ALEN; i++) {
        ped->ed_paddr[i] = dev->hwa[i];
        ped->ed_bcast[i] = ~0;
    }
    ped->ed_irq = dev->irq;
    return(OK);
}

int mon_3c905_ethintr() {
    STATWORD          ps;
    struct dev_3c905* dev;
    struct eth_pd*    epd;
    unsigned long     intst, txst;

    disable(ps);

    dev = &mon_dev_eth;

    _3CCMD(dev, _3C905_CMD_SETINTERRUPT, 0);

    /* get status */
    intst = inw(dev->iobase + _3C905_OFF_INTSTATUS);

    /* not our interrupt */
    if (! (intst & _3C905_FLG_INTERRUPT)) {
        _3CCMD(dev, _3C905_CMD_SETINTERRUPT, _3C905_MSK_INTERRUPTS);
        restore(ps);
        return(OK);
    }

    /* acknowledge interrupt */
    _3CCMD(dev, _3C905_CMD_ACKINTERRUPT, _3C905_FLG_INTERRUPT);

    while(1) {
        /* get status */
        intst = inw(dev->iobase + _3C905_OFF_INTSTATUS) & _3C905_MSK_INTERRUPTS;

#ifdef DEBUG
        kprintf("is=0x%x upp=0x%x dnp=0x%x\n",
                intst, 
                inl(dev->iobase + _3C905_OFF_UPLISTPTR),
                inl(dev->iobase + _3C905_OFF_DNLISTPTR));
#endif

        if (intst == 0)
            break;

        if (intst & _3C905_FLG_UPCOMPLINT) {
            _3CCMD(dev, _3C905_CMD_ACKINTERRUPT, _3C905_FLG_UPCOMPLINT);
            mon_3c905_ethdemux(dev);
        }
        if (intst & _3C905_FLG_TXCOMPLINT) {
            mon_3c905_ethxintr(dev);
            txst  = inb(dev->iobase + _3C905_OFF_TXSTATUS);
            outb(dev->iobase + _3C905_OFF_TXSTATUS, 1);
        }
        if (dev->state & _3C905_STT_RECLAIM) {
            while (1) {
                epd = &mon_eth_rxring[dev->rx_begin];
                if (epd->status == 0 || epd->buffer != 0) {
                    dev->state &= ~_3C905_STT_RECLAIM;
                    break;
                }
		epd->buffer = (void*) GETBUF(mon_Net.netpool);
                if (epd->buffer == (void *)0 || epd->buffer == (void *)SYSERR) {
                    epd->buffer = 0;
                    break;
                }
                epd->status = 0;
                epd->buffer = (void*) epd->buffer + sizeof(struct ehx);
                epd->length = _3C905_FLG_LASTFRAG | EP_MAXLEN;
                dev->rx_begin = (dev->rx_begin + 1) % _3C905_RXRING;
            }
        }
    }
#ifdef DEBUG
    kprintf("ethintr: leave\n");
#endif
    _3CCMD(dev, _3C905_CMD_SETINTERRUPT, _3C905_MSK_INTERRUPTS);
    restore(ps);
    return(OK);
}

int mon_3c905_ethdemux (dev) 
struct dev_3c905* dev;
{
    struct eth_pd* epd;
    struct ep*     pep;
    unsigned long  status;
    int len, nopackets;

    nopackets = MAX_RCV;

    while (nopackets--) {
        epd = &mon_eth_rxring[dev->rx_begin];
        status = epd->status;
        
        if (! (status & _3C905_FLG_UPCOMPLETE))
            break;

        if (status & _3C905_FLG_UPERROR) {
#ifdef DEBUG
            kprintf("ethdemux: packet error\n");
#endif
            epd->status = 0;
            epd->length = _3C905_FLG_LASTFRAG | EP_MAXLEN;
        } else {
            if (epd->buffer) {
                dev->state &= ~_3C905_STT_RECLAIM;
                len = status & _3C905_MSK_PKTLENGTH;
                pep = (struct ep*) (epd->buffer - sizeof(struct ehx));
                pep->ep_ifn  = dev->ifn;
                pep->ep_len  = len;
                pep->ep_type = net2hs(pep->ep_type);
#if 1
                mon_ni_in(&mon_nif[dev->ifn], pep, len);
#else
                freebuf(pep);
#endif
            }

            epd->buffer = (void*) GETBUF(mon_Net.netpool);
            if (epd->buffer == (void *)SYSERR || epd->buffer == (void *)0) {
#ifdef DEBUG
                kprintf("ethdemux: no buffer\n");
#endif
                epd->buffer = NULL;
                dev->state |= _3C905_STT_RECLAIM;
                break;
            }
            epd->status = 0;
            epd->buffer = (void*) epd->buffer + sizeof(struct ehx);
            epd->length = _3C905_FLG_LASTFRAG | EP_MAXLEN;
        }
        dev->rx_begin = (dev->rx_begin + 1) % _3C905_RXRING;
    }
    status = inl(dev->iobase + _3C905_OFF_UPPKTSTATUS);
    if (status & _3C905_FLG_UPSTALLED)
        _3CCMD(dev, _3C905_CMD_UPUNSTALL, 0);
    return(OK);
}

int mon_3c905_ethxintr(dev)
struct dev_3c905* dev;
{
    struct eth_pd* epd;
    struct ep*     pep;
    unsigned long  status;

    while(1) {
        epd = &mon_eth_txring[dev->tx_begin];
        status = epd->status;

        if (! (status & _3C905_FLG_DNCOMPLETE))
            break;

        status = inb(dev->iobase + _3C905_OFF_TXSTATUS);
        /* write to advance */
        outb(dev->iobase + _3C905_OFF_TXSTATUS, status);

        if (status & _3C905_MSK_TXERROR) {
#ifdef DEBUG
            kprintf("ethxintr: tx error\n");
#endif
            /* needs improvement */
            if (status & (_3C905_FLG_ERRTXSTATUSOVF|_3C905_FLG_ERRTXMAXCOLL)) {
                _3CCMD(dev, _3C905_CMD_TXENABLE, 0);
            }
        }

        epd->status = 0;
        pep = (struct ep*) (epd->buffer - sizeof(struct ehx));
        freebuf(pep);
        dev->tx_begin = (dev->tx_begin + 1) % _3C905_TXRING;
    }
    return(OK);
}

int mon_3c905_ethwrite (pep, len)
struct ep* pep;
int len;
{
    STATWORD          ps;
    struct dev_3c905* dev;
    struct eth_pd*    epd, *prev;

#ifdef DEBUG
    kprintf("ethwrite: \n");
#endif

    if (pep == NULL || len > EP_MAXLEN)
        return SYSERR;

    dev = &mon_dev_eth;

    /* update the packet */
    blkcopy(pep->ep_src, dev->hwa, EP_ALEN);
    pep->ep_len  = len;
    pep->ep_type = hs2net(pep->ep_type);

    /* get the dpd structure */
    disable(ps);

    if ((dev->tx_end + 2) % _3C905_TXRING == dev->tx_begin) {
        restore(ps);
#ifdef DEBUG
        kprintf("ethwrite: buffer full: %d %d\n", dev->tx_end, dev->tx_begin);
#endif
        freebuf(pep);
        return SYSERR;
    }

    prev        = &mon_eth_txring[dev->tx_end];
    dev->tx_end = (dev->tx_end + 1) % _3C905_TXRING;
    epd         = &mon_eth_txring[dev->tx_end];

    restore(ps);
 
    /* write the dpd structure */
    epd->next   = 0;
    epd->status = _3C905_FLG_TXINDICATE|_3C905_FLG_RNDUPDEFEAT | len;
    epd->buffer = ((void*) pep) + sizeof(struct ehx);
    epd->length = _3C905_FLG_LASTFRAG | len;

    /* send it to the nic */
    ethcmdwait(dev, _3C905_CMD_DNSTALL, 0);
    prev->next  = epd;
    prev        = (struct eth_pd*) inl(dev->iobase + _3C905_OFF_DNLISTPTR);
    if (! prev)
        outl(dev->iobase + _3C905_OFF_DNLISTPTR, (int) epd);
    _3CCMD(dev, _3C905_CMD_DNUNSTALL, 0);

    return OK;
}
