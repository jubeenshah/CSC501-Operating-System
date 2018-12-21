#include <./mon/monnetwork.h>
#include <./mon/moni386.h>
#include <./mon/moneepro.h>
#include <stdio.h>

/*#define DEBUG*/

/*------------------------------------------------------------------------
 * mon_ethcmd - issue an Intel 82586 command
 *------------------------------------------------------------------------
 */
int mon_ethcmd(iobase, cmd, istatus)
unsigned short  iobase, cmd, istatus;
{
    u_char  b;
    int i;

    /* make sure exec int bit is clear */
    outb(iobase + EEP_STATUS_REG, EEP_EXEC_INT);

    DELAY(1000);

    outb(iobase + EEP_COMMAND_REG, cmd);

    DELAY(1000);

    for (i = 0; i < 100; i++) {
	b = inb(iobase + EEP_STATUS_REG);
	if (b & EEP_EXEC_INT) {
	    b = inb(iobase + EEP_COMMAND_REG);
	    if ((b & EEP_CMD_STATUS_MASK) == istatus)
		return OK;
	    /* clear exec int bit */
	    outb(iobase + EEP_STATUS_REG, EEP_EXEC_INT);
	}
	else
	    DELAY(1000);
    }
    kprintf("eep: ** command %d failed **\n", cmd);
    return SYSERR;
}

