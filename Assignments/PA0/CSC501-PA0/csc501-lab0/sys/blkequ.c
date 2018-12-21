/* blkequ.c - blkequ */

#include <kernel.h>

/*------------------------------------------------------------------------
 *  blkequ  -  return TRUE iff one block of memory is equal to another
 *------------------------------------------------------------------------
 */
int blkequ(void *p1, void *p2, int nbytes)
{
	char *s1 = (char *)p1, *s2 = (char *)p2;
	while (--nbytes >= 0)
		if (*s1++ != *s2++)
			return(FALSE);
	return(TRUE);
}
