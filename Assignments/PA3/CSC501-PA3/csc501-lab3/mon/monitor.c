/* 
 * monitor.c - a simple monitor to handle the serial BREAK signal. 
 *             It allows users to boot Xinu, restart Xinu, or continue
 *	       execution.
 * 
 * Author:	John C. Lin
 * 		Dept. of Computer Science
 * 		Purdue University
 * Date:	Thu Jun 29 20:30:02 1995
 */
#include <./mon/moncom.h>
#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moni386.h>
#include <bufpool.h>
#include <stdio.h>

#define MON_PROMPT	"monitor> "
#define MON_MAXSTR	79
extern short girmask;
extern int console_dev;

struct  netinfo mon_Net;

/*-------------------------------------------------------------------------
 * monitor -
 * NOTE: assume all interrupts disabled.
 *-------------------------------------------------------------------------
 */
int monitor(u_short csr)
{
    int i = 0;
    unsigned char ch = 0, str[MON_MAXSTR+1];

    kprintf("monitor commands: <?> help, <b> boot Xinu, <c> continue, <r> restart\n");
    kprintf("%s", MON_PROMPT);
    
    while (1) {
        ch = inb(csr+UART_LSR);	/* get status */
	
	if (ch & UART_LSR_BI) {	/* ignore BREAK */
	    (void) inb(csr+UART_RX);
	    kprintf("%s", MON_PROMPT);
	    continue;
	}
	
	if (!(ch & UART_LSR_DR))
	    continue;	/* loop till data is ready */
	
	ch = inb(csr+UART_RX);

	switch (ch) {
	case '\0':				/* null */
	    kprintf("%s", MON_PROMPT);
            continue;
	
	case '\r':
	    if (i > 0) {
		kputc(console_dev, ch);
		kputc(console_dev, '\n');	/* new line */
		str[i] = '\0';
		if (mon_cmd(str))
		    return(OK);			/* continue */
		i = 0;
	    }
	    else
		kprintf("\n%s", MON_PROMPT);
	    continue;
	    
	case '\b':			/* backspace */
	    if (i > 0) {
		kputc(console_dev, '\b');
		kputc(console_dev, ' ');
		kputc(console_dev, '\b');
		i--;
	    }
	    break;

	default:
	    kputc(console_dev, ch);
	    if (i < MON_MAXSTR)
		str[i++] = ch;
	    break;
	}
    }
}

/*-------------------------------------------------------------------------
 * mon_cmd - process a monitor command
 *-------------------------------------------------------------------------
 */
int mon_cmd(char *str)
{
    STATWORD ps;
    short old_girmask;
    extern int   mon_clkint();	/* clock int. handler, for retx purpose */
    extern int   clkint();
    
#ifdef DEBUG
    kprintf("mon_cmd = [%s]\n", str);
#endif
    if (strcmp(str, "b") == 0) {
	old_girmask = girmask;	/* save it */
	
	/*
	 * init ethernet interface, ARP, nif[] structures, and interrupts
	 */
	mon_Net.netpool = mkpool(MON_MAXNETBUF, MON_NETBUFS);
	mon_ethinit();
	mon_arpinit();
	mon_netinit();
	
	kprintf("booting Xinu....\n");
	if (boot_xinu() == SYSERR) {
	    /*
	     * If booting failed, disable all interrupts, restore original
	     * clock interrupt handler, restore original int. mask, and
	     * return to monitor.
	     */
	    disable(ps);
	    set_evec((u_int) IRQBASE, (u_long) clkint);
	    girmask = old_girmask;
	    kprintf("sorry, failed to boot Xinu from network.\n");
	    kprintf("%s", MON_PROMPT);
	}
    }
    else if (strcmp(str, "c") == 0) {
	return 1;
    }
    else if (strcmp(str, "r") == 0) {
	start();
    }
    else if ((strcmp(str, "h") == 0) || (strcmp(str, "?") == 0) ||
	(strcmp(str, "help") == 0)) {
	mon_help();
	kprintf("%s", MON_PROMPT);
    }
    else {
	kprintf("unknown command: [%s], enter 'h' for help.\n", str);
	kprintf("%s", MON_PROMPT);
    }
    
    return 0;
}

/*-------------------------------------------------------------------------
 * mon_help - print monitor help menu
 *-------------------------------------------------------------------------
 */
int mon_help()
{
    kprintf("\n---------------------------------\n");
    kprintf("?,h\thelp\n");
    kprintf("b\tboot Xinu from network\n");
    kprintf("c\tcontinue execution\n");
    kprintf("r\trestart Xinu\n");
    kprintf("---------------------------------\n\n");
    return(OK);
}
