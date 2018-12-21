/* clkint.s - _clkint */

#include <icu.s>
		.text
count1000:	.word	1000
		.globl	clkint
clkint:
		cli
		pushal
		movb	$EOI,%al
		outb	%al,$OCW1_2

		incl	ctr1000
		subw	$1,count1000
		ja	cl1
		incl	clktime
		movw	$1000,count1000
cl1:
		cmpl	$0,slnempty
		je	clpreem
		movl	sltop,%eax
		decl	(%eax)
		jg	clpreem      /* need jg since sltop signed */
		call	wakeup
clpreem:	decl	preempt
		jg	clret        /* need jg since preempt signed */
		call	resched
clret:
		popal
		sti
		iret
