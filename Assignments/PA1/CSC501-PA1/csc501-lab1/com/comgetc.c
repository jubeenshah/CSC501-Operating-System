
#include <conf.h>
#include <kernel.h>
#include <io.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  comgetc -- read a character from a serial line
 *------------------------------------------------------------------------
 */
int comgetc(struct devsw * pdev)
{
	kprintf("COMGETC:\n");
        return(OK);
}

/*------------------------------------------------------------------------
 *  comread -- read from a serial line
 *------------------------------------------------------------------------
 */
int comread(struct devsw * pdev, char *  buf, int count)
{
	kprintf("COMREAD:\n");
        return(OK);
}
