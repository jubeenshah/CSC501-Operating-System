#
#  Makefile for CSC501 Xinu.
#

INCLUDE = -I../h
CFLAGS  = -m32 -march=i586 -fno-builtin -c -Wall -O ${DEFS} ${INCLUDE}
SDEFS	= -I../h
ASFLAGS = --32

CC      =       /usr/bin/gcc
cPP     =       /usr/bin/gcc -E
AS      =       /usr/bin/as
LD      =       /usr/bin/ld
MAKETD  =	/usr/X11R6/bin/makedepend
AWK	=	awk
LIB     =       ../lib
XINU    =       ./xinu

# Amount to move loaded image down in memory
BRELOC  =	0x100000
BOOTPLOC=	0x150000

#------------------------------------------------------------------------
# source files
#------------------------------------------------------------------------
COM =	comcntl.c	comgetc.c	comiin.c	cominit.c	\
	cominput.c	comoutput.c	comread.c	comintr.c

MON =   monitor.c       monarp.c        monbootp.c      monip.c         \
	monnet.c        monudp.c        mongpq.c        ethintr.c       \
	ethwrite.c      ethinit.c       ethdemux.c      ethwstrt.c      \
	ethrom.c        monboot.c       montimer.c      ethcmd.c	\
	monpci.c	mon3com.c

SYS =	blkcmp.c	blkequ.c	main.c		stacktrace.c	\
	chprio.c	clkinit.c	close.c		conf.c		\
	control.c	create.c	evec.c		freebuf.c	\
	freemem.c	getbuf.c	getc.c		getitem.c	\
	getmem.c	getpid.c	getprio.c	getstk.c	\
	gettime.c	gpq.c		i386.c		init.c		\
	insert.c	insertd.c	ioerr.c		ionull.c	\
	kill.c		kprintf.c	kputc.c		mark.c		\
	mkpool.c	newqueue.c	open.c		panic.c		\
	poolinit.c	putc.c		queue.c		read.c		\
	ready.c		receive.c	recvclr.c	recvtim.c	\
	resched.c	resume.c	scount.c	screate.c	\
	sdelete.c	send.c		setdev.c	setnok.c	\
	signal.c	signaln.c	sleep.c		sleep10.c	\
	sleep100.c	sleep1000.c	sreset.c	suspend.c	\
	unsleep.c	userret.c	wait.c		wakeup.c	\
	write.c		xdone.c		pci.c           shutdown.c

TTY =	ttyalloc.c	ttycntl.c	ttygetc.c	ttyiin.c	\
	ttyinit.c	ttynew.c	ttyopen.c	ttyputc.c	\
	ttyread.c	ttywrite.c	ttyoin.c

PG  =   get_bs.c        release_bs.c    read_bs.c       write_bs.c      \
        control_reg.c   bsm.c           policy.c        \
        frame.c         pfint.c         dump32.c        vcreate.c       \
        xm.c            vgetmem.c       vfreemem.c

SRC = ${COM} ${TTY} ${MON} ${SYS}

#------------------------------------------------------------------------
# object files
#------------------------------------------------------------------------
COMOBJ = ${COM:%.c=%.o}

MONOBJ = ${MON:%.c=%.o}

SYSOBJ = ${SYS:%.c=%.o}

TTYOBJ = ${TTY:%.c=%.o}

PGOBJ = ${PG:%.c=%.o}

XOBJ = startup.o initialize.o intr.o clkint.o ctxsw.o pfintr.o

OBJ =	${COMOBJ} ${MONOBJ} ${SYSOBJ} ${TTYOBJ}		\
	${PGOBJ}					\
	moncksum.o monclkint.o comint.o ethint.o montftp.o

#------------------------------------------------------------------------
# make targets
#------------------------------------------------------------------------
all: xinu.elf

xinu.elf: Makefile Configuration ../h/conf.h ${XOBJ} ${OBJ} ${LIB}/libxc.a
	$(LD) -m elf_i386 -dn -Ttext 0x10000 -e start ${XOBJ} ${OBJ} ${LIB}/libxc.a \
	-o ${XINU}.elf

clean: FRC
	rm -rf .d* *.o *.errs *.bak *nm* core ${XINU} ${XINU}.elf tags version
	rm -rf ../h/conf.h
	echo '0' > vn
	(cd ${LIB}/libxc; ${MAKE} clean)

depend: makedep
	sed -e '1,/^# DO NOT DELETE THIS LINE/!d' Makefile > Makefile.base
	cat Makefile.base makedep > Makefile
	rm Makefile.base makedep makedep1 makedeperrs
	@rm -f ../h/conf.h

undepend:
	sed -e '1,/^# DO NOT DELETE THIS LINE/!d' Makefile > Makefile.base
	mv Makefile.base Makefile

CFILES = ../*/*.c ../*/*/*.c

makedep: FRC
	@cat /dev/null >makedep
	@touch ../h/conf.h
	-(for i in ${CFILES} ; do \
		${CC} -M -w ${DEFS} -I../h ./$$i >> makedep; done) \
		2>makedeperrs
	sed -e 's,^\(.*\)\.o:,\1.o:,' makedep > makedep1
	${AWK} ' { if ($$1 != prev) { print rec; rec = $$0; prev = $$1; } \
		else { if (length(rec $$3) > 78) { print rec; rec = $$0; } \
			else rec = rec " " $$3 } } \
		END { print rec } ' makedep1 > makedep
	@cat makedeperrs
	@(if [ -s makedeperrs ]; then false; fi)

xdepend: FRC
	${MAKETD} ${DEFS} -I../h ${CFILES}

run:
	@qemu-system-i386 -nographic -device isa-debug-exit,iobase=0xf4,iosize=0x04 -kernel ${XINU}.elf ||:

debug:
	@qemu-system-i386 -s -S -nographic -device isa-debug-exit,iobase=0xf4,iosize=0x04 -kernel ${XINU}.elf ||:

#------------------------------------------------------------------------
#
#------------------------------------------------------------------------
tags: FRC
	ctags -R ..

../config/config:
	make -C ../config/ -f ../config/Makefile clean all

../h/conf.h ../sys/conf.c: ../config/config Configuration
	../config/config; mv conf.c ../sys 

${LIB}/libxc.a: FRC
	(cd ${LIB}/libxc; make install)

intr.o: ../sys/intr.S
	${CPP} ${SDEFS} ../sys/intr.S | ${AS} ${ASFLAGS} -o intr.o

clkint.o: ../sys/clkint.S
	${CPP} ${SDEFS} ../sys/clkint.S | ${AS} ${ASFLAGS} -o clkint.o

ctxsw.o: ../sys/ctxsw.S
	${CPP} ${SDEFS} ../sys/ctxsw.S | ${AS} ${ASFLAGS} -o ctxsw.o

pfintr.o: ../paging/pfintr.S
	${CPP} ${SDEFS} ../paging/pfintr.S | ${AS} ${ASFLAGS} -o pfintr.o

ethint.o: ../mon/ethint.S
	${CPP} ${SDEFS} ../mon/ethint.S | ${AS} ${ASFLAGS} -o ethint.o

startup.o: ../sys/startup.S Makefile
	${CPP} ${SDEFS} -DBRELOC=${BRELOC} -DBOOTPLOC=${BOOTPLOC} \
	../sys/startup.S | ${AS} ${ASFLAGS} -o startup.o 

moncksum.o: ../mon/moncksum.S
	${CPP} ${SDEFS} ../mon/moncksum.S | ${AS} ${ASFLAGS} -o moncksum.o

monclkint.o: ../mon/monclkint.S
	${CPP} ${SDEFS} ../mon/monclkint.S | ${AS} ${ASFLAGS} -o monclkint.o

montftp.o: ../mon/montftp.c
	${CC} ${CFLAGS} -DBOOTPLOC=${BOOTPLOC} ../mon/montftp.c -o montftp.o

comint.o: ../com/comint.S
	${CPP} ${SDEFS} ../com/comint.S | ${AS} ${ASFLAGS} -o comint.o

initialize.o: $(OBJ) startup.o 
	sh mkvers.sh
	${CC} -c ${CFLAGS} -DVERSION=\""`cat version`"\" ../sys/initialize.c

${MONOBJ}:
	 ${CC} ${CFLAGS} ../mon/`basename $@ .o`.[c]
${COMOBJ}:
	 ${CC} ${CFLAGS} ../com/`basename $@ .o`.[c]
${SYSOBJ}:
	 ${CC} ${CFLAGS} ../sys/`basename $@ .o`.[c]
${TTYOBJ}:
	 ${CC} ${CFLAGS} ../tty/`basename $@ .o`.[c]	 
${PGOBJ}:
	${CC} ${CFLAGS} ../paging/`basename $@ .o`.[c]

FRC:
#------------------------------------------------------------------------

# DO NOT DELETE THIS LINE - maketd DEPENDS ON IT

comcntl.o: ../com/comcntl.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h
comgetc.o: ../com/comgetc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h ../h/stdio.h
comiin.o: ../com/comiin.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/com.h
cominit.o: ../com/cominit.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/com.h ../h/stdio.h
cominput.o: ../com/cominput.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/io.h ../h/stdio.h
comintr.o: ../com/comintr.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/com.h ../h/stdio.h
comoutput.o: ../com/comoutput.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/tty.h ../h/com.h \
  ../h/stdio.h
comread.o: ../com/comread.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
lex.yy.o: ../config/lex.yy.c ../h/stdio.h /usr/include/string.h \
  /usr/include/features.h /usr/include/sys/cdefs.h \
  /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h \
  /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/errno.h /usr/include/bits/errno.h \
  /usr/include/linux/errno.h /usr/include/asm/errno.h \
  /usr/include/asm-generic/errno.h /usr/include/asm-generic/errno-base.h \
  /usr/include/stdlib.h /usr/include/sys/types.h \
  /usr/include/bits/types.h /usr/include/bits/typesizes.h \
  /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
  /usr/include/sys/select.h /usr/include/bits/select.h \
  /usr/include/bits/sigset.h /usr/include/bits/time.h \
  /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h \
  /usr/include/alloca.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h
y.tab.o: ../config/y.tab.c /usr/include/stdlib.h /usr/include/features.h \
  /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h \
  /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h \
  /usr/lib/gcc/i386-redhat-linux/4.1.2/include/stddef.h \
  /usr/include/sys/types.h /usr/include/bits/types.h \
  /usr/include/bits/typesizes.h /usr/include/time.h /usr/include/endian.h \
  /usr/include/bits/endian.h /usr/include/sys/select.h \
  /usr/include/bits/select.h /usr/include/bits/sigset.h \
  /usr/include/bits/time.h /usr/include/sys/sysmacros.h \
  /usr/include/bits/pthreadtypes.h /usr/include/alloca.h ../h/stdio.h \
  ../config/lex.yy.c /usr/include/string.h /usr/include/errno.h \
  /usr/include/bits/errno.h /usr/include/linux/errno.h \
  /usr/include/asm/errno.h /usr/include/asm-generic/errno.h \
  /usr/include/asm-generic/errno-base.h /usr/include/unistd.h \
  /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
  /usr/include/getopt.h
ethcmd.o: ../mon/ethcmd.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/moni386.h \
  ../h/./mon/moneepro.h ../h/stdio.h
ethdemux.o: ../mon/ethdemux.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/moneepro.h \
  ../h/stdio.h
ethinit.o: ../mon/ethinit.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/moneepro.h \
  ../h/./mon/mon3com.h ../h/./mon/monintel.h ../h/./mon/monpci.h \
  ../h/stdio.h
ethintr.o: ../mon/ethintr.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/moneepro.h \
  ../h/stdio.h
ethrom.o: ../mon/ethrom.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/moneepro.h \
  ../h/stdio.h
ethwrite.o: ../mon/ethwrite.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/stdio.h
ethwstrt.o: ../mon/ethwstrt.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/moneepro.h \
  ../h/stdio.h
mon3com.o: ../mon/mon3com.c ../h/mon/moni386.h ../h/mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/mon/mon3com.h \
  ../h/mon/monpci.h ../h/mon/monitor.h ../h/./mon/moncom.h \
  ../h/./mon/monnetwork.h ../h/./mon/monitor.h ../h/./mon/moni386.h \
  ../h/stdio.h
monarp.o: ../mon/monarp.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/stdio.h
monboot.o: ../mon/monboot.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/stdio.h
monbootp.o: ../mon/monbootp.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/monbootp.h \
  ../h/stdio.h
mongpq.o: ../mon/mongpq.c ../h/./mon/monconf.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/monnetwork.h ../h/./mon/monsystypes.h \
  ../h/./mon/monip.h ../h/./mon/monether.h ../h/./mon/monudp.h \
  ../h/./mon/monbootp.h ../h/./mon/monarp.h ../h/./mon/monnetif.h \
  ../h/./mon/moni386.h ../h/stdio.h
monip.o: ../mon/monip.c ../h/./mon/monnetwork.h ../h/./mon/monsystypes.h \
  ../h/./mon/monconf.h ../h/./mon/monip.h ../h/./mon/monether.h \
  ../h/./mon/monudp.h ../h/./mon/monbootp.h ../h/./mon/monarp.h \
  ../h/./mon/monnetif.h ../h/./mon/monitor.h ../h/./mon/moncom.h \
  ../h/./mon/moni386.h ../h/stdio.h
monitor.o: ../mon/monitor.c ../h/./mon/moncom.h ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moni386.h ../h/bufpool.h ../h/mark.h ../h/stdio.h
monnet.o: ../mon/monnet.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/moni386.h \
  ../h/./mon/monitor.h ../h/./mon/moncom.h ../h/bufpool.h ../h/mark.h \
  ../h/stdio.h
monpci.o: ../mon/monpci.c ../h/mon/monnetwork.h ../h/./mon/monsystypes.h \
  ../h/./mon/monconf.h ../h/./mon/monip.h ../h/./mon/monether.h \
  ../h/./mon/monudp.h ../h/./mon/monbootp.h ../h/./mon/monarp.h \
  ../h/./mon/monnetif.h ../h/mon/mon3com.h ../h/mon/monpci.h \
  ../h/mon/monitor.h ../h/./mon/moncom.h ../h/./mon/monnetwork.h \
  ../h/./mon/monitor.h ../h/./mon/moni386.h ../h/stdio.h
montftp.o: ../mon/montftp.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/montftp.h \
  ../h/./mon/monbootp.h ../h/stdio.h
montimer.o: ../mon/montimer.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/stdio.h
monudp.o: ../mon/monudp.c ../h/./mon/monnetwork.h \
  ../h/./mon/monsystypes.h ../h/./mon/monconf.h ../h/./mon/monip.h \
  ../h/./mon/monether.h ../h/./mon/monudp.h ../h/./mon/monbootp.h \
  ../h/./mon/monarp.h ../h/./mon/monnetif.h ../h/./mon/monitor.h \
  ../h/./mon/moncom.h ../h/./mon/moni386.h ../h/./mon/montftp.h \
  ../h/./mon/monbootp.h ../h/stdio.h
bs.o: ../paging/bs.c ../h/conf.h ../h/kernel.h ../h/systypes.h ../h/mem.h \
  ../h/paging.h ../h/i386.h ../h/proc.h ../h/stdio.h
control_reg.o: ../paging/control_reg.c ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/i386.h ../h/stdio.h
frame.o: ../paging/frame.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
get_bs.o: ../paging/get_bs.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
page_table.o: ../paging/page_table.c ../h/stdio.h ../h/conf.h \
  ../h/kernel.h ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h \
  ../h/i386.h
pfint.o: ../paging/pfint.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/paging.h ../h/i386.h ../h/proc.h
policy.o: ../paging/policy.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/paging.h ../h/i386.h
read_bs.o: ../paging/read_bs.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/mark.h ../h/bufpool.h ../h/proc.h \
  ../h/paging.h ../h/i386.h
release_bs.o: ../paging/release_bs.c ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
vcreate.o: ../paging/vcreate.c ../h/stdio.h ../h/conf.h ../h/i386.h \
  ../h/kernel.h ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h \
  ../h/sem.h ../h/io.h
vfreemem.o: ../paging/vfreemem.c ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
vgetmem.o: ../paging/vgetmem.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
write_bs.o: ../paging/write_bs.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h \
  ../h/mark.h ../h/bufpool.h
xm.o: ../paging/xm.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
blkcmp.o: ../sys/blkcmp.c
blkequ.o: ../sys/blkequ.c ../h/kernel.h ../h/systypes.h ../h/conf.h \
  ../h/mem.h
chprio.o: ../sys/chprio.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
clkinit.o: ../sys/clkinit.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/sleep.h ../h/i386.h ../h/stdio.h ../h/q.h
close.o: ../sys/close.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
conf.o: ../sys/conf.c ../h/conf.h
control.o: ../sys/control.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
create.o: ../sys/create.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/sem.h ../h/io.h ../h/proc.h \
  ../h/paging.h ../h/i386.h
evec.o: ../sys/evec.c ../h/conf.h ../h/i386.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/sem.h \
  ../h/tty.h ../h/q.h ../h/io.h ../h/stdio.h
freebuf.o: ../sys/freebuf.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/mark.h ../h/bufpool.h ../h/stdio.h
freemem.o: ../sys/freemem.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/stdio.h
getbuf.o: ../sys/getbuf.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/mark.h ../h/bufpool.h ../h/stdio.h
getc.o: ../sys/getc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
getitem.o: ../sys/getitem.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/q.h
getmem.o: ../sys/getmem.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/stdio.h
getpid.o: ../sys/getpid.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
getprio.o: ../sys/getprio.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
getstk.o: ../sys/getstk.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/stdio.h
gettime.o: ../sys/gettime.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/date.h
gpq.o: ../sys/gpq.c ../h/conf.h ../h/kernel.h ../h/systypes.h ../h/mem.h \
  ../h/q.h ../h/stdio.h
i386.o: ../sys/i386.c ../h/icu.h ../h/i386.h ../h/kernel.h \
  ../h/systypes.h ../h/conf.h ../h/mem.h
init.o: ../sys/init.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
initialize.o: ../sys/initialize.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h \
  ../h/sem.h ../h/sleep.h ../h/tty.h ../h/q.h ../h/io.h
insert.o: ../sys/insert.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/q.h
insertd.o: ../sys/insertd.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/q.h
ioerr.o: ../sys/ioerr.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h
ionull.o: ../sys/ionull.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h
kill.o: ../sys/kill.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/sem.h ../h/io.h \
  ../h/q.h ../h/stdio.h
kprintf.o: ../sys/kprintf.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
kputc.o: ../sys/kputc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
main.o: ../sys/main.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
mark.o: ../sys/mark.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/mark.h
mkpool.o: ../sys/mkpool.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/mark.h ../h/bufpool.h ../h/stdio.h
newqueue.o: ../sys/newqueue.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/q.h
open.o: ../sys/open.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
panic.o: ../sys/panic.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
pci.o: ../sys/pci.c ../h/conf.h ../h/kernel.h ../h/systypes.h ../h/mem.h \
  ../h/pci.h ../h/stdio.h
poolinit.o: ../sys/poolinit.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/mark.h ../h/bufpool.h ../h/stdio.h
putc.o: ../sys/putc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
queue.o: ../sys/queue.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/q.h
read.o: ../sys/read.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
ready.o: ../sys/ready.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h
receive.o: ../sys/receive.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
recvclr.o: ../sys/recvclr.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
recvtim.o: ../sys/recvtim.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
resched.o: ../sys/resched.c ../h/stdio.h ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h \
  ../h/q.h
resume.o: ../sys/resume.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
scount.o: ../sys/scount.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/sem.h
screate.o: ../sys/screate.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
sdelete.o: ../sys/sdelete.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
send.o: ../sys/send.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
setdev.o: ../sys/setdev.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h
setnok.o: ../sys/setnok.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/stdio.h
signal.o: ../sys/signal.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
signaln.o: ../sys/signaln.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
sleep1000.o: ../sys/sleep1000.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
sleep100.o: ../sys/sleep100.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
sleep10.o: ../sys/sleep10.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
sleep.o: ../sys/sleep.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
sreset.o: ../sys/sreset.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
ssclock.o: ../sys/ssclock.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h
stacktrace.o: ../sys/stacktrace.c ../h/conf.h ../h/kernel.h \
  ../h/systypes.h ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h \
  ../h/stdio.h
suspend.o: ../sys/suspend.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/stdio.h
unsleep.o: ../sys/unsleep.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h \
  ../h/stdio.h
userret.o: ../sys/userret.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h
wait.o: ../sys/wait.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sem.h \
  ../h/stdio.h
wakeup.o: ../sys/wakeup.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/proc.h ../h/paging.h ../h/i386.h ../h/q.h ../h/sleep.h
write.o: ../sys/write.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h
xdone.o: ../sys/xdone.c ../h/kernel.h ../h/systypes.h ../h/conf.h \
  ../h/mem.h ../h/stdio.h
ttyalloc.o: ../tty/ttyalloc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/stdio.h
ttycntl.o: ../tty/ttycntl.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/stdio.h
ttygetc.o: ../tty/ttygetc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttyiin.o: ../tty/ttyiin.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttyinit.o: ../tty/ttyinit.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttynew.o: ../tty/ttynew.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttyoin.o: ../tty/ttyoin.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/stdio.h
ttyopen.o: ../tty/ttyopen.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttyputc.o: ../tty/ttyputc.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
ttyread.o: ../tty/ttyread.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h ../h/stdio.h
ttywrite.o: ../tty/ttywrite.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/tty.h
abs.o: ../lib/libxc/abs.c
atof.o: ../lib/libxc/atof.c ../h/ctype.h
atoi.o: ../lib/libxc/atoi.c
atol.o: ../lib/libxc/atol.c
bzero.o: ../lib/libxc/bzero.c
ctype_.o: ../lib/libxc/ctype_.c ../h/ctype.h
doprnt.o: ../lib/libxc/doprnt.c
doscan.o: ../lib/libxc/doscan.c ../h/ctype.h
ecvt.o: ../lib/libxc/ecvt.c
fgets.o: ../lib/libxc/fgets.c
fprintf.o: ../lib/libxc/fprintf.c
fputs.o: ../lib/libxc/fputs.c
gets.o: ../lib/libxc/gets.c
index.o: ../lib/libxc/index.c
printf.o: ../lib/libxc/printf.c
puts.o: ../lib/libxc/puts.c
qsort.o: ../lib/libxc/qsort.c
rand.o: ../lib/libxc/rand.c
rindex.o: ../lib/libxc/rindex.c
scanf.o: ../lib/libxc/scanf.c ../h/conf.h ../h/kernel.h ../h/systypes.h \
  ../h/mem.h ../h/io.h ../h/tty.h
sprintf.o: ../lib/libxc/sprintf.c
strcat.o: ../lib/libxc/strcat.c
strchr.o: ../lib/libxc/strchr.c
strcmp.o: ../lib/libxc/strcmp.c
strcpy.o: ../lib/libxc/strcpy.c
strlen.o: ../lib/libxc/strlen.c
strncat.o: ../lib/libxc/strncat.c
strncmp.o: ../lib/libxc/strncmp.c
strncpy.o: ../lib/libxc/strncpy.c
swab.o: ../lib/libxc/swab.c
