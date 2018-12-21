/*printsegaddress.c - printing address of segments (Task 2)*/

#include <stdio.h>
extern int etext,edata,end;

/*----------------------------------------------------------------------------------------------
 *  *  printsegaddress  -  print address indicating the end of text, data, and BSS(end) segments
 *   *---------------------------------------------------------------------------------------------
 *    */ /*As per the man pages at https://linux.die.net/man/3/etext
                                  these segments should be explicitly declared and not defined
                                  in any header files*/

void printsegaddress() {
	
	kprintf("\nvoid printsegaddress()\n");
	kprintf("\n1. Current:");
	kprintf("  eText[0x%08x] : 0x%08x, ",&etext,etext);
	kprintf("  eData[0x%08x] : 0x%08x, ",&edata,edata);
	kprintf("  BSS(end)[0x%08x] : 0x%08x ",&end,end);
	
	/*
	printf("\nvoid printsegaddress()\n");
        printf("\n1. Current:");
        printf("  eText[0x%08x] : 0x%08x, ",&etext,*(&etext));
        printf("  eData[0x%08x] : 0x%08x, ",&edata,*(&edata));
        printf("  BSS(end)[0x%08x] : 0x%08x ",&end,*(&end));
	*/
	//int address_Txt = (int *)&etext;
	//int address_dta = (int *)&edata;
	//int address_end = (int *)&end;
	kprintf("\n2. Preeceeding: ");
        kprintf("  eText [0x%08x] : 0x%08x,",((int *)&etext - 1),*((int *)&etext -1));
        kprintf("  eData [0x%08x]: 0x%08x,",((int *)&edata - 1),*((int *)&etext - 1));
        kprintf("  BSS(end) [0x%08x] : 0x%08x",((int *)&end - 1),*((int *)&etext - 1));


        kprintf("\n3. After:");
        kprintf("  eText [0x%08x]: 0x%08x, ",((int *)&etext + 1),*((int *)&etext + 1));
        kprintf("  eData [0x%08x]: 0x%08x, ",((int *)&edata + 1),*((int *)&edata + 1));
        kprintf("  BSS(end)[0x%08x] : 0x%08x",((int *)&end + 1),*((int *)&end + 1));
}
