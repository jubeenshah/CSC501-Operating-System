/* fputs.c - fputs */

/*------------------------------------------------------------------------
 *  fputs  --  write a null-terminated string to a device (file)
 *------------------------------------------------------------------------
 */
fputs(int dev, char *s)
{
	int r, c;
	int putc();

	while (c = *s++)
                r = putc(dev, c);
	return(r);
}
