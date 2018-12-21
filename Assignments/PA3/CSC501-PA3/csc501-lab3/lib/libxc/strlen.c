/*
 * Returns the number of
 * non-NULL bytes in string argument.
 */

int strlen(const char *s)
{
	int n;

	n = 0;
	while (*s++)
		n++;

	return(n);
}
