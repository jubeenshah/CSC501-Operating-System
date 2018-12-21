#include <icu.s>

		.data
count100:	.word	100
		.text
		.globl	mon_clkint
mon_clkint:
		cli
		pushal

		subw	$1,count100
		ja	clret
		incl	mon_clktime
		call	mon_timer
		movw	$100,count100
clret:
		movb    $EOI,%al
		outb    %al,$OCW1_2
		popal
		sti
		iret
