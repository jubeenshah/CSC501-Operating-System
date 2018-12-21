/* ctxsw.s - ctxsw */

/* DO NOT MODIFY THIS FILE UNTIL THE APPROPRIATE LAB */
	
		.text
		.globl	ctxsw
aglobal:	.word	0

/*------------------------------------------------------------------------
 * ctxsw -  call is ctxsw(&oldsp, &oldmask, &newsp, &newmask)
 *------------------------------------------------------------------------
 */
ctxsw:

	/*----------------------------------------------------------------
	 *
	 * For a futurelab you will insert your version of ctsw here.
	 *
	 * Because this function is obviously critical, in the compile
	 * directory is ctxsw.o. A backup copy is also in in this (sys) 
	 * directoy.
	 *
	 * Should this file, ctxsw.S get a newer timestamp than 
	 * compile/ctxsw.o (e.g. you resave this file) simply "touch 
	 * compile/ctxsw.o" to give it a newer timestamp.
	 *
	 *----------------------------------------------------------------
	 */
		
	ret

