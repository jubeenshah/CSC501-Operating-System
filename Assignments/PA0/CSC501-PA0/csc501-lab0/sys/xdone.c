/* xdone.c - xdone */
#include <kernel.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * xdone  --  print system completion message as last process exits
 *------------------------------------------------------------------------
 */
int xdone()
{
    kprintf("\n\nAll user processes have completed.\n\n");
    return(OK);
}
