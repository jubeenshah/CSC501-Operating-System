/* ctype.h - isalpha, isupper, islower, isdigit, isspace, ispunct,	*/
/*		isalnum, isprshort, isprint, iscntrl, toupper, tolower,	*/
/*		toascii							*/

#ifndef _CTYPE_H_
#define _CTYPE_H_

#define	_U	0001
#define	_L	0002
#define	_N	0004
#define _D      _N      /* for the embedded libc */
#define	_S	0010
#define _P	0020
#define _C	0040
#define	_X	0100

extern	char	_ctype_[]; /* in {Xinu-directory}/src/libc/gen/ctype_.h	*/

#define	isalpha(c)	((_ctype_+1)[(int)c]&(_U|_L))
#define	isupper(c)	((_ctype_+1)[(int)c]&_U)
#define	islower(c)	((_ctype_+1)[(int)c]&_L)
#define	isdigit(c)	((_ctype_+1)[(int)c]&_N)
#define	isxdigit(c)	((_ctype_+1)[(int)c]&(_N|_X))
#define	isspace(c)	((_ctype_+1)[(int)c]&_S)
#define ispunct(c)	((_ctype_+1)[(int)c]&_P)
#define isalnum(c)	((_ctype_+1)[(int)c]&(_U|_L|_N))
#define isprshort(c)	((_ctype_+1)[(int)c]&(_P|_U|_L|_N))
#define isprint(c)	((_ctype_+1)[(int)c]&(_P|_U|_L|_N|_S))
#define iscntrl(c)	((_ctype_+1)[(int)c]&_C)
#define isascii(c)	((unsigned)((int)(c))<=0177)
#define toupper(c)	(((int)(c))-'a'+'A')
#define tolower(c)	(((int)(c))-'A'+'a')
#define toascii(c)	(((int)(c))&0177)

#define ishexnumber(c) (isdigit(c) || \
                (isalpha(c) && c >= (isupper(c) ? 'A' : 'a') && \
		c <= (isupper(c) ? 'F' : 'f')))

#endif
