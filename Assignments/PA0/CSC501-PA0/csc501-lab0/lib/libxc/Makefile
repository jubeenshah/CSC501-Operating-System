#
#  Make the Xinu version of the C run-time support library
#

CC	=	/usr/bin/gcc
AS	=	/usr/bin/as
#CFLAGS	=	-march=i586 -m32 -fno-builtin -O -c -I../../h
CFLAGS =        -m32 -fno-builtin -O -c -I../../h
SFLAGS	=	-I../../h
AR	=	/usr/bin/ar
LIB	=	..


.s.o:
		$(AS) --32 -o $@ $(AFLAGS) $<

.c.o:
		$(CC) $(CFLAGS) $<

CFILES	=	abs.c atof.c atoi.c atol.c blkcopy.s bzero.c ctype_.c	\
		doprnt.c doscan.c ecvt.c fgets.c fprintf.c fputs.c 	\
		gets.c index.c printf.c puts.c qsort.c rand.c rindex.c 	\
		scanf.c	sprintf.c strcat.c strchr.c strcmp.c strcpy.c	\
		strlen.c strncat.c strncmp.c strncpy.c swab.c

OFILES	=	abs.o atof.o atoi.o atol.o blkcopy.o bzero.o ctype_.o	\
		doprnt.o doscan.o ecvt.o fgets.o fprintf.o fputs.o	\
		gets.o index.o printf.o puts.o qsort.o rand.o rindex.o	\
		scanf.o	sprintf.o strcat.o strchr.o strcmp.o strcpy.o	\
		strlen.o strncat.o strncmp.o strncpy.o swab.o

all:		libxc.a

libxc.a:	${OFILES}
		rm -f libxc.a
		ar cr libxc.a $(OFILES)

install:	libxc.a
		cp libxc.a $(LIB)/libxc.a

clean:
		rm -f ${OFILES} libxc.a $(LIB)/libxc.a
