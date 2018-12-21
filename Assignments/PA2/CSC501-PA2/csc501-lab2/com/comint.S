/* comint.s - comint */

#include <icu.s>

/*------------------------------------------------------------------------
 * comint  --  interrupt handler for SLU input
 *------------------------------------------------------------------------
 */
	.text
	.globl	comint

comint:
	cli
	pushal

	movb	$EOI,%al
	outb	%al,$OCW1_2
	call	comintr

	popal
	sti
	iret
