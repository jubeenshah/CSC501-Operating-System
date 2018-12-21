/* intr.s - disable(), restore(), enable(), pause(), halt() */


#include <icu.s> 

	.text
	.globl	enable
	.globl	disable
	.globl	restore
	.globl	pause
	.globl	halt
	.globl	getirmask
	.globl	spurious_irq7
	.globl	spurious_irq15

/*------------------------------------------------------------------------
 * Xtrap - trap entry
 *------------------------------------------------------------------------
 */
Xtrap:
	call	trap
	movb	$OCW1_2,%al
	outb	%al,$EOI
	popl	%eax		/* exception # */
	testl	$35,%eax
	jb	3f
	shrl	$5,%eax		/* convert to IRQ # */

	/* issue global rearm to cards */
	testl	$7,%eax
	ja	1f
	orl	$0x2f0,%eax	/* 0x2fX for IRQ 3-7 */
	movb	%bl,(%eax)
	jmp	3f
1:	testl	$9,%eax
	jz	2f
	testl	$15,%eax
	ja	3f
	orl	$0x6f0,%eax	/* 0x2f(X-8) for IRQ 10-15 */
	movb	%bl,(%eax)
	jmp	3f
2:	movb	%al,0x2f2	/* 0x2f2 for IRQ 9 */
3:
	popal
	leave
	iret

/*------------------------------------------------------------------------
 * _Xint? - default exception and interrupt entry points
 *	NB: These must be contiguous
 *------------------------------------------------------------------------
 */
#include "../sys/xint.s"

		.globl	defevec
defevec:
		.long	_Xint0
		.long	_Xint1
		.long	_Xint2
		.long	_Xint3
		.long	_Xint4
		.long	_Xint5
		.long	_Xint6
		.long	_Xint7
		.long	_Xint8
		.long	_Xint9
		.long	_Xint10
		.long	_Xint11
		.long	_Xint12
		.long	_Xint13
		.long	_Xint14
		.long	_Xint15
		.long	_Xint16
		.long	_Xint17
		.long	_Xint18
		.long	_Xint19
		.long	_Xint20
		.long	_Xint21
		.long	_Xint22
		.long	_Xint23
		.long	_Xint24
		.long	_Xint25
		.long	_Xint26
		.long	_Xint27
		.long	_Xint28
		.long	_Xint29
		.long	_Xint30
		.long	_Xint31
		.long	_Xint32
		.long	_Xint33
		.long	_Xint34
		.long	_Xint35
		.long	_Xint36
		.long	_Xint37
		.long	_Xint38
		.long	spurious_irq7
		.long	_Xint40
		.long	_Xint41
		.long	_Xint42
		.long	_Xint43
		.long	_Xint44
		.long	_Xint45
		.long	_Xint46
		.long	spurious_irq15

/*---------------------------------------------------------
 * pause: halt the processor until an interrupt occurs
 *---------------------------------------------------------
 */
pause:
	call enable
	hlt
	ret


/*---------------------------------------------------------
 * halt: do nothing forever
 *---------------------------------------------------------
 */
halt:
	jmp halt
	ret


/*---------------------------------------------------------
 * enable all interrupts
 *---------------------------------------------------------
 */
enable:
	cli
	movw	girmask,%ax
	outb	%al,$IMR1
	shrw	$8,%ax
	outb	%al,$IMR2
	sti
	ret


/*---------------------------------------------------------
 * disable(ps)    - disable interrupts, save old state in ps
 * STATWORD ps  (short *ps)
 *---------------------------------------------------------
 */
disable:
	cli
	pushfl
	inb	$IMR2,%al
	shlw	$8,%ax
	inb	$IMR1,%al	
	notw	%ax		/* clear _girmask bits for user */
	orw	girmask,%ax
	notw	%ax
	movl	8(%esp),%edx	/* get PS pointer	*/
	movw	%ax,(%edx)	/* save old IR mask	*/
	movb	$0xff,%al
	outb	%al,$IMR2
	movb	$0xff,%al
	outb	%al,$IMR1
	popfl
	ret


/*---------------------------------------------------------
 * restore(ps)    - restore interrupts to value in ps
 * STATWORD ps    (short *ps)
 *---------------------------------------------------------
 */
restore:
        cli
	pushfl
        movl    8(%esp),%edx
        movw    (%edx),%ax
        orw     girmask,%ax
        outb    %al,$IMR1
        shrw    $8,%ax
        outb    %al,$IMR2
	popfl
	sti
        ret

/*------------------------------------------------------------------------
 * getirmask(ps)  - return current interrupt mask in ps
 *------------------------------------------------------------------------
 */
getirmask:
	inb	$IMR2,%al
	shlw	$8,%ax
	inb	$IMR1,%al	
	movl	4(%esp),%edx
	movw	%ax,(%edx)
	ret

spurious_irq7:
	mov	$EOI,%al
	outb	%al,$OCW1_2
	iret

spurious_irq15:
	mov	$EOI,%al
	outb	%al,$OCW1_2
	mov	$EOI,%al
	outb	%al,$OCW2_2
	iret
