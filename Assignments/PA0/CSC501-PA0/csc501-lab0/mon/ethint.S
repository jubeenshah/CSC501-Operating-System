#include <icu.s>

		.text
		.globl	mon_ethint
mon_ethint:
		cli
		pushal

		movb	$EOI,%al	/* re-enable the device */
		outb	%al,$OCW1_2

		call	mon_ethintr

		popal
		sti
		iret

		.globl	mon_ethint_hi
mon_ethint_hi:				/* for the 3com cards */
		cli
		pushal

		movb	$EOI,%al	/* re-enable the device */
		outb	%al,$OCW1_2
		movb	$EOI,%al
		outb	%al,$OCW2_2

		call	mon_3c905_ethintr

		popal
		sti
		iret

