
#include <conf.h>
#include <kernel.h>
#include <io.h>

/*------------------------------------------------------------------------
 *  congetc -- read a character from the SUN physical keyboard
 *------------------------------------------------------------------------
 */
int congetc(struct devsw *devptr )
{
    return(OK);
}


/*------------------------------------------------------------------------
 *  conread -- read from the SUN physical keyboard
 *------------------------------------------------------------------------
 */
int conread(struct devsw * devptr, char * buff, int count)
{
    return(OK);
}
