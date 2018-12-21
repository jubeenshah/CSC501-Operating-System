/* ttyoin.c - ttyoin */

#include <conf.h>
#include <kernel.h>
#include <tty.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * ttyoin - handle transmit interrupt on tty
 *------------------------------------------------------------------------
 */
int
ttyoin(ptty, ch)
struct tty	*ptty;
unsigned char	ch;
{
	kprintf("ttyoin\n");
        return(OK);
}
