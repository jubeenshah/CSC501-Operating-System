/*
 * Author: John C. Lin (lin@cs.purdue.edu).
 * Date: 08/02/95
 *
 * Ref: 1. Linux driver by Bao C. Ha (bao@saigon.async.com)
 *	2. Intel Windows NDIS driver for EE PRO/10
 *
 * Note: This version uses Port-I/O.
 */

#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moni386.h>
#include <./mon/moneepro.h>
#include <./mon/mon3com.h>
#include <./mon/monintel.h>
#include <./mon/monpci.h>
#include <stdio.h>

struct ethdev    mon_eth[1];

extern int mon_ethwrite(struct ep*, int);
static int eep_probe(struct ethdev *ped, u_short iobase);
static int search_eepro(u_short iobase);
static int check_board(u_short iobase);
static int check_mem_size(u_short iobase);
static int i82595_init(struct ethdev *ped, u_short iobase);
static int config_593(u_short iobase);

/*#define DEBUG*/
/*#define VERBOSE*/

/*------------------------------------------------------------------------
 * mon_ethinit - startup initialization of Intel EtherExpress device
 *------------------------------------------------------------------------
 */
int mon_ethinit()
{
    struct ethdev *ped;
    u_short iobase;

    ped = &mon_eth[0];
    ped->ed_outq = mon_newq(16);
    ped->ed_iobase = 0;		/* auto search */

    iobase = ped->ed_iobase;

    /*
     * Check for PCI BIOS
     */
#ifdef DEBUG
    kprintf("Checking PCI\n");
#endif
    if (mon_pci_init() == OK) {
	kprintf("ethinit: Found PCI BIOS\n");
	if (mon_find_pci_device(_3COM_3C905_DEVICE_ID, _3COM_VENDOR_ID, 0) == OK) {
	    /* found the 3com 3C905 card */
	    kprintf("ethinit: Found 3COM 3c905 card, configuring.\n");
	    mon_3c905_ethinit();
	    return OK;
	} else if (mon_find_pci_device(_INTEL_PRO100_DEVICE_ID, _INTEL_VENDOR_ID, 0) == OK) {
	    /* found the Intel Pro/100 card */
	    kprintf("ethinit: Intel Pro/100 S found -- can't configure\n");
	} else {
	    /* no recognized cards */
	    kprintf("ethinit: No recognized PCI ethernet cards found\n");
	}
    }
    /*
     * probe device
     */
    if (eep_probe(ped, iobase) == SYSERR) {
        kprintf("** No EtherExpress PRO/10 card at 0x%x **\n", iobase);
        return SYSERR;
    }

    /*
     * Initialize i82595
     */
    i82595_init(ped, ped->ed_iobase);

    /*
     * Set the vector to the proper write routine
     */
    mon_nif[0].ni_write=mon_ethwrite;

#ifdef DEBUG
    kprintf("eep: init ok\n");
#endif
    return(OK);
}

/* Intel's Ethernet address prefix */
#define INTEL_ETH0 0x00
#define INTEL_ETH1 0xaa
#define INTEL_ETH2 0x00

/*-------------------------------------------------------------------------
 * eep_probe - check board ID and verify checksum
 *-------------------------------------------------------------------------
 */
static int eep_probe(struct ethdev *ped, u_short iobase)
{
    u_short      i,w;
    int irq = 0, mon_ethint();

    if ((w = search_eepro(iobase)) == 0)
	return SYSERR;

    if (w != iobase) {
#ifdef VERBOSE
	kprintf("eep: set IOBASE = 0x%x\n", w);
#endif
	ped->ed_iobase = iobase = w;
    }
    
    /*
     * Found an EE PRO/10 card. Read station address from the EEPROM
     */
    w = mon_ethromread(iobase, 4);
    ped->ed_paddr[0] = w >> 8;
    ped->ed_paddr[1] = w & 0xff;
    w = mon_ethromread(iobase, 3);
    ped->ed_paddr[2] = w >> 8;
    ped->ed_paddr[3] = w & 0xff;
    w = mon_ethromread(iobase, 2);
    ped->ed_paddr[4] = w >> 8;
    ped->ed_paddr[5] = w & 0xff;

    if (ped->ed_paddr[0] != INTEL_ETH0 || ped->ed_paddr[1] != INTEL_ETH1 ||
	ped->ed_paddr[2] != INTEL_ETH2)
	return SYSERR;
    
    for (i = 0; i < EP_ALEN; i++)
	ped->ed_bcast[i] = ~0;		/* bcast address */

#ifdef DEBUG
    kprintf("eep_probe: EtherExpress PRO/10 at iobase = 0x%x\n", iobase);
#endif
    
    /*
     * read connector type
     */
#ifdef VERBOSE
    outb(iobase, EEP_BANK2_SELECT);
    b = inb(iobase + EEP_REG3);
    if (b & EEP_TPE_BIT)
	kprintf("eep: Connector type TPE\n");
    else
	kprintf("eep: Connector type BNC\n");
#endif

    /*
     * read IRQ number
     */
    w = mon_ethromread(iobase, 1);
    switch (w & 0x07) {
    case 0: irq = 9; break;
    case 1: irq = 3; break;
    case 2: irq = 5; break;
    case 3: irq = 10; break;
    case 4: irq = 11; break;
    default: /* should never get here !!!!! */
	kprintf("eep_probe: illegal interrupt vector stored in EEPROM.\n");
	return SYSERR;
    }

    ped->ed_irq = irq;
    set_evec(ped->ed_irq+IRQBASE, (unsigned int) mon_ethint);
    
#ifdef VERBOSE
    kprintf("eep: Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x ",
            ped->ed_paddr[0] & 0xff,
            ped->ed_paddr[1] & 0xff,
            ped->ed_paddr[2] & 0xff,
            ped->ed_paddr[3] & 0xff,
            ped->ed_paddr[4] & 0xff,
            ped->ed_paddr[5] & 0xff);
    kprintf("irq %d iobase 0x%x\n", ped->ed_irq, ped->ed_iobase);
#endif

    /*
     * find out the size of the on-board memorry
     */
    switch (check_mem_size(iobase)) {
    case 32:	/* 32K RAM */
#ifdef VERBOSE
	kprintf("eep: 32K on-board RAM\n");
#endif
	ped->ed_mem_size = EEP_RAM_SIZE_32;
	ped->ed_txbuf_size = EEP_TX_SIZE_32;
	break;

    case 64:
#ifdef VERBOSE
	kprintf("eep: 64K on-board RAM\n");
#endif
	ped->ed_mem_size = EEP_RAM_SIZE_64;
	ped->ed_txbuf_size = EEP_TX_SIZE_64;
	break;

    default:
	kprintf("eep: ** error on-board RAM test failed **\n");
	return SYSERR;
    }
    
    ped->ed_rxbuf_size = ped->ed_mem_size - ped->ed_txbuf_size;
    
    return OK;
}

/*-------------------------------------------------------------------------
 * search_eepro -
 *-------------------------------------------------------------------------
 */
static int search_eepro(u_short iobase)
{
    int i;
    int io;

    if ((io = check_board(iobase)) != SYSERR)
	return io;
	
    /*
     * search for the card
     */
    iobase = 0x200;
    for (i = 0; i <= 7; i++) {
	if ((io = check_board(iobase)) != SYSERR)
	    return io;
	iobase += 0x10;
    }
    
    /*
     * search for the card, again.
     */
    iobase = 0x300;
    for (i = 0; i <= 7; i++) {
	if ((io = check_board(iobase)) != SYSERR)
	    return io;
	iobase += 0x10;
    }
    return 0;
}

/*-------------------------------------------------------------------------
 * check_board
 *-------------------------------------------------------------------------
 */
static int check_board(u_short iobase)
{
    u_char i, b, shift_cnt;

#ifdef DEBUG
    kprintf("check_board: iobase=%x\n", iobase);
#endif
    
    /*
     * check board ID
     */
    for (i = 0; i < 4; i++) {
	b = inb(iobase + EEP_ID_REG) & EEP_ID_REG_MASK;
	if (b == EEP_ID_REG_SIG)
	    break;
    }

    shift_cnt = 0x40;

    for (i = 0; i < 3; i++) {
	b = inb(iobase + EEP_ID_REG) & EEP_ID_REG_MASK;

	if (b != (EEP_ID_REG_SIG + shift_cnt))
	    return SYSERR;

	shift_cnt += 0x40;
    }

    return (iobase);
}


/*-------------------------------------------------------------------------
 * check_mem_size - check on-board RAM size
 * return value: 32: 32K, 64: 64K, SYSERR
 *-------------------------------------------------------------------------
 */
static int check_mem_size(u_short iobase)
{
    u_short i, j, k;
    
    u_short io_port = iobase + EEP_IO_PORT;
    u_short addr_port = iobase + EEP_HOST_ADDRESS_REG;
    
    outb(iobase, EEP_BANK0_SELECT);	/* select bank 0 */

    /*
     * set I/O address to 0; do read & write test
     */
    outw(addr_port, 0);
    j = 3;
    for (i = 0; i < 16; i++) {	/* write a pattern */
	outw(io_port, j);
	j += 3;
    }
    
    outw(addr_port, 0);
    j = 3;
    for (i = 0; i < 16; i++) {	/* read it back and compare */
	k = inw(io_port);

	if (k != j)
	    return SYSERR;
	j += 3;
    }

    /*
     * write to upper memory to see if we have 32K or 64K RAM
     */
    outw(addr_port, 0x8000);
    j = 9;
    for (i = 0; i < 16; i++) {	/* write a pattern */
	outw(io_port, j);
	j += 3;
    }

    /*
     * zero out lower memory in case of a 32K wrap
     */
    outw(addr_port, 0);
    for (i = 0; i < 16; i++) {	/* write a pattern */
	outw(io_port, 0);
    }

    /*
     * now check to see if we have a 64K RAM
     */
    outw(addr_port, 0x8000);
    j = 9;
    for (i = 0; i < 16; i++) {	/* read it back and compare */
	k = inw(io_port);

	if (k != j)
	    return 32;		/* we have 32K RAM */
	j += 3;
    }

    return 64;			/* we have 64K RAM */
}



static char irqrmap[] = {-1,-1,0,1,-1,2,-1,-1,-1,0,3,4,-1,-1,-1,-1};
/*-------------------------------------------------------------------------
 * i82595_init - initialize i82595
 *-------------------------------------------------------------------------
 */
static int i82595_init(struct ethdev *ped, u_short iobase)
{
    u_char b;
    int	 i;

    /* RESET the 82595 */
    if (mon_ethcmd(iobase, EEP_RESET_CMD, EEP_RESET_CMD) == SYSERR) {
	return SYSERR;
    }
    
    /*
     * switch to bank 1; set limit registers, irq, etc.
     */
    outb(iobase, EEP_BANK1_SELECT);

    /*
     * un-tristate interrupt (do not change host bus width)
     */
    b = inb(iobase + EEP_REG1);
    outb(iobase + EEP_REG1, b | EEP_TRI_ST_INT);

    /*
     * set IRQ
     */
    b = inb(iobase + EEP_INT_SEL_REG);
    outb(iobase + EEP_INT_SEL_REG, (b & 0xf8) | irqrmap[ped->ed_irq]);
    /*
     * Enable the interrupt line.
     */
    b = inb(iobase + EEP_REG1);
    outb(iobase + EEP_REG1, b | EEP_INT_ENABLE);

    /*
     * Initialize the TX and RX buffer lower and upper bounds
     */
    outb(iobase + EEP_XMT_LOWER_LIMIT_REG, 0);
    outb(iobase + EEP_XMT_UPPER_LIMIT_REG, (ped->ed_txbuf_size >> 8) - 1);
    outb(iobase + EEP_RCV_LOWER_LIMIT_REG, (ped->ed_txbuf_size >> 8));
    outb(iobase + EEP_RCV_UPPER_LIMIT_REG, ((ped->ed_mem_size) >> 8) - 1);

    /*
     * Switch to BANK 2
     */
    outb(iobase, EEP_BANK2_SELECT);

    /*
     * Setup Transmit Chaining and discard bad RCV frames
     */
    b = inb(iobase + EEP_REG1);
    outb(iobase + EEP_REG1, b | EEP_XMT_Chain_Int | EEP_RCV_Disc_BadFrame);

    /*
     * No source address insertation and CRC not in memory
     */
    outb(iobase + EEP_REG2, EEP_CRC_MEM | EEP_NO_SA_INS);

    /*
     * auto-port detect and zero out test bits
     */
    b = inb(iobase + EEP_REG3);
    outb(iobase + EEP_REG3, b & (~(EEP_AUTO_PORT|EEP_TEST2|EEP_TEST1)));

    /*
     * Switch to BANK 0
     */
    outb(iobase, EEP_BANK0_SELECT);

    /* select reset and wait for at least 2 us */
    outb(iobase, EEP_SEL_RESET_CMD);
    DELAY(100);

    /* Initialize TX BAR */
    outw(iobase + EEP_XMT_BAR, 0);
    ped->ed_tx_start = ped->ed_tx_end = 0;
    
    /* Initialize RCV CAR/BAR, stop register */
    outw(iobase + EEP_RCV_BAR, ped->ed_txbuf_size);
    outw(iobase + EEP_RCV_STOP, ped->ed_mem_size - 1);
    ped->ed_rx_start = 	ped->ed_txbuf_size;

    /* issue 593 configure command to disable auto-stripping of frames */
    config_593(iobase);

    /*
     * diag 82595
     */
    if (mon_ethcmd(iobase, EEP_DIAGNOSE_CMD, EEP_DIAGNOSE_CMD) == SYSERR)
	return SYSERR;

    /*
     * set station address, Switch to BANK 2
     */
    outb(iobase, EEP_BANK2_SELECT);

    
    for (i = 0; i < EP_ALEN; i++)
        outb(iobase+EEP_I_ADD_REG0+i, ped->ed_paddr[i]);

    /*
     * back to BANK 0
     */
    outb(iobase, EEP_BANK0_SELECT);
    
    /*
     * Enable RX and TX interrupts
     */
    outb(iobase+EEP_INT_MASK_REG, EEP_ALL_MASK & ~(EEP_RX_MASK | EEP_TX_MASK));

    /*
     * clear all interrupts
     */
    outb(iobase + EEP_STATUS_REG, EEP_ALL_MASK);

    outb(iobase, EEP_RCV_ENABLE_CMD);

    return OK;
}


/*-------------------------------------------------------------------------
 * config_593 - This is an undocumented command and should not be tampered
 *		with 
 *-------------------------------------------------------------------------
 */
static int config_593(u_short iobase)
{
    outw(iobase+EEP_XMT_BAR, 0);
    outw(iobase+EEP_HOST_ADDRESS_REG, 0);
    outw(iobase+EEP_IO_PORT, 2);	/* 593 config op-code */
    outw(iobase+EEP_IO_PORT, 0);	/* status */
    outw(iobase+EEP_IO_PORT, 0);	/* chain pointer */
    outw(iobase+EEP_IO_PORT, 0x0a);	/* byte count */
    
    outw(iobase+EEP_IO_PORT, 0x2e);
    outw(iobase+EEP_IO_PORT, 0x60);	/* IFS and slot time */
    outw(iobase+EEP_IO_PORT, 0x8f2);
    outw(iobase+EEP_IO_PORT, 0x4000);
    outw(iobase+EEP_IO_PORT, 0xb6);	/* normally is 0xb4 */

    outb(iobase+EEP_COMMAND_REG, 0x02);

    DELAY(1000);
    
    outw(iobase+EEP_XMT_BAR, 0);	/* reset tx pointer reg */
    outw(iobase+EEP_HOST_ADDRESS_REG, 0); /* reset host addr reg */
    outb(iobase+EEP_STATUS_REG, 0x08);	/* ack the exec int bit */
    return(OK);
}
