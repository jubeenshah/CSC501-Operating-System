/* ctxsw.S - Context switch routine */


		.text
newmask:	.long	0

		.globl	ctxsw

/*------------------------------------------------------------------------
 * ctxsw -  call is ctxsw(&oldsp, &oldmask, &newsp, &newmask)
 *------------------------------------------------------------------------
 */
ctxsw:
	pushl %ebp		# Save EBP and set up the new stack frame
	movl %esp, %ebp		#

	pushl 0xC(%ebp)		# Push the address of oldmask onto the stack as the argument for disable(PS),
				# which will load the current interrupt mask into it...
	call disable		# ...and then actually call disable 

	movl 0x14(%ebp), %eax	# Load the address of newmask into EAX
	movw (%eax), %dx	# Load the value at newmask (dereference EAX) into EAX
	movw %dx, newmask	# Load the value of newmask into the newmask data field
	
	pushf			# Save register and flags at this point for when we switch back to
	pushal			# this (the old process) next time
	
	# Now do the actual context switch
	movl 0x8(%ebp), %eax	# Move the address of oldsp into EAX
	movl %esp, (%eax)	# Save the stack pointer into oldsp
	
	movl 0x10(%ebp), %eax	# Move the address of newsp into EAX
	movl (%eax), %esp	# Restore the stack pointer from newsp
	
	popal			# Restore the registers and flags from last time this process was switched away from
	popf
	
	pushl $newmask		# Push newmask onto the stack for the restore routine to set as the interrupt mask...
	call restore		# ...and then actually call restore
	
	leave			# Set ESP to EBP, then pop EBP (undo the top 2 instructions in this routine)
	ret

