/* pfintr.S - pfintr */


    	   .text
pferrcode: .long 0
           .globl  pfintr,pferrcode
pfintr:
      popl pferrcode
      pushfl
      cli
      pushal
      call pfint
      popal
      popfl
      iret
