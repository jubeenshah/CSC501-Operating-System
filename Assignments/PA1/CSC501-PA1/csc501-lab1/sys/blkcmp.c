/* blkcmp.c - blkcmp */

/*------------------------------------------------------------------------
 * blkcmp - do a lexical compare of a block of len bytes
 *------------------------------------------------------------------------
 */
int blkcmp(void *p1, void *p2, int len)
{
	int 	i, rv = 0;
	char *s1 = (char *)p1, *s2 = (char *)p2;

	for (i = 0; i < len; ++i, ++s1, ++s2)
		if ((rv = *s1 - *s2))
			break;
	return rv;
}
