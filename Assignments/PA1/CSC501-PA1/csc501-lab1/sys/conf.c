/* conf.c (GENERATED FILE; DO NOT EDIT) */

#include <conf.h>

/* device independent I/O switch */

struct	devsw	devtab[NDEVS] = {

/*  Format of entries is:
device-number, device-name,
init, open, close,
read, write, seek,
getc, putc, cntl,
device-csr-address, input-vector, output-vector,
iint-handler, oint-handler, control-block, minor-device,
*/

/*  CONSOLE  is tty  */

{ 0, "CONSOLE",
ttyinit, ttyopen, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
0000000, 0000, 0000,
ttyiin, ttyoin, NULLPTR, 0 },

/*  SERIAL0  is serial  */

{ 1, "SERIAL0",
cominit, ionull, ionull,
ionull, comwrite, ioerr,
comgetc, computc, comcntl,
0001770, 0044, 0000,
comint, ioerr, NULLPTR, 0 },

/*  SERIAL1  is serial  */

{ 2, "SERIAL1",
cominit, ionull, ionull,
ionull, comwrite, ioerr,
comgetc, computc, comcntl,
0001370, 0043, 0000,
comint, ioerr, NULLPTR, 1 },

/*  TTY0  is tty  */

{ 3, "TTY0",
ttyinit, ttyopen, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
0000000, 0000, 0000,
ttyiin, ttyoin, NULLPTR, 1 },

/*  TTY1  is tty  */

{ 4, "TTY1",
ttyinit, ttyopen, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
0000000, 0000, 0000,
ttyiin, ttyoin, NULLPTR, 2 },

/*  TTY2  is tty  */

{ 5, "TTY2",
ttyinit, ttyopen, ionull,
ttyread, ttywrite, ioerr,
ttygetc, ttyputc, ttycntl,
0000000, 0000, 0000,
ttyiin, ttyoin, NULLPTR, 3 }
	};
