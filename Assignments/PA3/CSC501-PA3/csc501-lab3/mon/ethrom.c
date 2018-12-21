#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <./mon/moni386.h>
#include <./mon/moneepro.h>
#include <stdio.h>

#define	ROMDELAY()	{ DELAY(1); }

#define EEPROM_READ_CMD (6 << 6)

/*------------------------------------------------------------------------
 * mon_ethromread - read data from the serial EEPROM
 * Note: the 82595 will be set to Bank 0 after the eeprom is read
 *------------------------------------------------------------------------
 */
unsigned short mon_ethromread(iobase, from)
short	iobase;		/* base I/O address		*/
short	from;		/* EEPROM offset (words)	*/
{
    int   i;
    short loc = iobase + EEP_EEPROM_REG;
    int   read_cmd = from | EEPROM_READ_CMD;
    unsigned short retval = 0;

    outb(iobase, EEP_BANK2_SELECT);
    outb(loc, EEP_EECS);

    /* Shift the read command bits out. */
    for (i = 8; i >= 0; i--) {
        short w = (read_cmd & (1 << i)) ? EEP_EECS | EEP_EEDI : EEP_EECS;
        outb(loc, w);
        outb(loc, w | EEP_EESK);   /* EEPROM clock tick. */
        ROMDELAY();
        outb(loc, w);		/* Finish EEPROM a clock tick. */
	ROMDELAY();
    }
    outb(loc, EEP_EECS);

    for (i = 16; i > 0; i--) {
        outb(loc, EEP_EECS | EEP_EESK);
	ROMDELAY();
        retval = (retval << 1) | ((inb(loc) & EEP_EEDO) ? 1 : 0);
        outb(loc, EEP_EECS);
	ROMDELAY();
    }

    /* Terminate the EEPROM access. */
    outb(loc, EEP_EESK);
    ROMDELAY();
    outb(loc, 0);
    ROMDELAY();
    outb(iobase, EEP_BANK0_SELECT);
    return retval;
}
