#include <./mon/monnetwork.h>
#include <./mon/monitor.h>
#include <stdio.h>

/*#define PRINTERR*/
/*-------------------------------------------------------------------------
 * mon_timer - called once per second from clock interrupt routine (mon_clkint).
 *-------------------------------------------------------------------------
 */
int mon_timer()
{
    if (mon_timeout && mon_clktime >= mon_timeout) {
	switch (mon_boot_state) {
	case BOOTP_REQ_SENT:
	    mon_boot_state = BOOTP_RETX;
	    break;

	case TFTP_REQ_SENT:
	    if (mon_tftp_bytes == 0)
	    	mon_boot_state = TFTP_RETX;
	    else {
		if (mon_tftp_retx < 5) {
#ifdef PRINTERR
		   kprintf("mon_timer: RETX TFTP ACK=%d\n", mon_tftp_block-1);
#endif
		   mon_tftp_ack(mon_tftp_block-1);
		   mon_tftp_retx++;
		}
		else {
		   kprintf("mon_timer: too many retransmissions\n");
		   mon_boot_state = BOOT_ERROR;
		   mon_timeout = 0;
		}
	    }
	    break;
	    
	default:
	    break;
	}
    }
    return(OK);
}
