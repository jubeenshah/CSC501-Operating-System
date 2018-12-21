/* shutdown.c - shutdown */
#include <kernel.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  exit  --  power down the system
 *  requires support using special qemu command line parameters
 *  http://wiki.osdev.org/Shutdown
 *------------------------------------------------------------------------
 */
int shutdown()
{
    outb(0xf4, 0x00);
    return(OK);
}
