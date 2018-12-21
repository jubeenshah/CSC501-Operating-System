/* Returns pointer to character, or NULL if not found */
char *strchr (const char *s, int c)
{
	for (; *s != '\0'; s++)
		if (*s == (const char)c)
			return (char *)s;
	return 0;
}
