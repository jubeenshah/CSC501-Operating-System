.code32
.section        .data
.section        .text

.globl zfunction
zfunction:
	pushl %ebp 
	movl %esp, %ebp 		
	movl 0x8(%ebp), %eax  
	andl $0xfffc01ff, %eax  
	shr $0x8, %eax
	add $0xff000000, %eax		
	popl %ebp
	ret
