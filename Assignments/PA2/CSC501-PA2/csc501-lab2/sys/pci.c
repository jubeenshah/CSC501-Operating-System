/* pci.c
 *
 * Benjamin A. Kuperman
 *
 * Some information here is modeled after that seen in the linux
 * source tree.
 */

#include <conf.h>
#include <kernel.h>
#include <pci.h>
#include <stdio.h>

/*
#define DEBUG
*/

SYSCALL pcibios_init(void);
SYSCALL check_pcibios(void);
/*------------------------------------------------------------------------

	      ####   #        ####   #####     ##    #        ####
	     #    #  #       #    #  #    #   #  #   #       #
	     #       #       #    #  #####   #    #  #        ####
	     #  ###  #       #    #  #    #  ######  #            #
	     #    #  #       #    #  #    #  #    #  #       #    #
	      ####   ######   ####   #####   #    #  ######   ####

 *------------------------------------------------------------------------
 */

static bios32	*bios32ptr;	/* pointer to the BIOS 32 structure */
static unsigned long bios32_service(unsigned long service);

/* from linux */
static unsigned long bios32_entry = 0;
static struct {
    unsigned long address;
    unsigned short segment;
} bios32_indirect = { 0, 0 + 5*0x08 /* 0x10  KERNEL_CS */ };

static long pcibios_entry = 0;
static struct {
    unsigned long address;
    unsigned short segment;
} pci_indirect = { 0, 0+5*0x08 /* 0x10  KERNEL_CS */ };

/*------------------------------------------------------------------------
 * pci_init - initialize the PCI bios structures and such
 *------------------------------------------------------------------------
 */
SYSCALL pci_init(void)
{
    if (OK == pcibios_init()) {
	kprintf("pci_init: PCI BIOS initialization ");
	kprintf("succeeded\n");
	return OK;
    } else {
	kprintf("pci_init: PCI BIOS initialization ");
	kprintf("failed\n");
	return SYSERR;
    }
}

/*------------------------------------------------------------------------
 * pcibios_init
 *------------------------------------------------------------------------
 */
SYSCALL pcibios_init(void)
{
    unsigned char   *ptr;
    bios32	    *bptr;
    int		    i,length;
    unsigned char   cksum;

    /* search for it */
    for (bptr = (bios32 *)BIOS32_MIN_ADDR; 
	 bptr <= (bios32 *)BIOS32_MAX_ADDR; bptr++) {
	/* check for signature first */
	if (BIOS32_SIGNATURE != bptr->signature)
	    continue;
	/* now check the checksum */
	cksum=0;
	length = bptr->length * BIOS32_UNITS;
	ptr=(char *)bptr;
	for (i=0; i<length; i++) {
	    cksum += ptr[i];
	}
	if (0 != cksum) {
	    kprintf("pcibios_init: invalid checksum at 0x%08lx\n",bptr);
	    continue;
	}
	/* verify the revision */
	if (0 != bptr->revision) {
	    kprintf("pcibios_init: invalid revision %d at 0x%08lx\n",
		    bptr->revision,bptr);
	    continue;
	}
	/* If I am here, then all tests were valid */
	bios32ptr = bptr;		/* set the global reference */

#ifdef DEBUG
	kprintf("pcibios_init: BIOS32 Service Directory has been found at 0x%08lx\n",bios32ptr);
	kprintf("pcibios_init: BIOS32 Service Directory entry point is at 0x%08lx\n",bios32ptr->bios32_entry);
#endif

	/* for the stuff from linux */
	bios32_entry = bios32ptr->bios32_entry;
	bios32_indirect.address = bios32_entry;

	if (check_pcibios()) {
	    return OK;
	} else {
	    return SYSERR;
	}
    }

    /* If I am here, nothing was found */
    kprintf("pcibios_init: no BIOS32 Service Directory was found\n");
    return SYSERR;

}


/*------------------------------------------------------------------------
 * check_pcibios - taken from linux source
 *------------------------------------------------------------------------
 */
SYSCALL check_pcibios(void)
{
    unsigned long signature;                                                        unsigned char present_status;
    unsigned char major_revision;
    unsigned char minor_revision;
    unsigned long flags;
    int pack;

    if ((pcibios_entry = bios32_service(PCI_SERVICE))) {
	pci_indirect.address = pcibios_entry;
	save_flags(flags);
	cli();
	__asm__(  "lcall *(%%edi)\n\t"
		  "jc 1f\n\t"
		  "xorb %%ah, %%ah\n"
		  "1:\tshl $8, %%eax\n\t"
		  "movw %%bx, %%ax"
		: "=d" (signature),
		  "=a" (pack)
		: "1" (PCIBIOS_PCI_BIOS_PRESENT),
		  "D" (&pci_indirect)
		: "bx", "cx");
	restore_flags(flags);

	present_status = (pack >> 16) & 0xff;
	major_revision = (pack >> 8) & 0xff;
	minor_revision = pack & 0xff;
	if (present_status || (signature != PCI_SIGNATURE)) {
	    kprintf ("pcibios_init : %s : BIOS32 Service Directory says PCI BIOS is present,\n"
		    "       but PCI_BIOS_PRESENT subfunction fails with present status of 0x%x\n"
		    "       and signature of 0x%08lx (%c%c%c%c).\n",
		    (signature == PCI_SIGNATURE) ?  "WARNING" : "ERROR",
		    present_status, signature,
		    (char) (signature >>  0), (char) (signature >>  8),
		    (char) (signature >> 16), (char) (signature >> 24));

	    if (signature != PCI_SIGNATURE)
		pcibios_entry = 0;
	}
	if (pcibios_entry) {
	    kprintf ("pcibios_init : PCI BIOS revision %x.%02x entry at 0x%lx\n",
		    major_revision, minor_revision, pcibios_entry);
	    return 1;
	}
    }
    return 0;
}

static unsigned long bios32_service(unsigned long service)
{
    unsigned char return_code;      /* %al */
    unsigned long address;          /* %ebx */
    unsigned long length;           /* %ecx */
    unsigned long entry;            /* %edx */
    unsigned long flags;

    /* According to the PCI BIOS specification, the following needs to
     * take place before the "CALL FAR":
     *
     * 1. The CS code segment selector and the DS data segment
     * selector must be set up to encompass the physical page holding
     * the entry point *as well as* the immediately following physical
     * page.
     *
     * 2. Them must also have the same base.
     *
     * 3. Platform BIOS writers must assume that CS is execute-only
     * and DS is read-only.
     *
     * 4. The SS stack segment selectore must provide at least 1K of
     * stack space.
     */

    save_flags(flags); 
    cli();
    asm(    "nop\n"
	    "nop\n"
	    "lcall *(%%edi)\n"
	    : "=a" (return_code),
	    "=b" (address),
	    "=c" (length),
	    "=d" (entry)
	    : "0" (service),
	    "1" (0),
	    "D" (&bios32_indirect));
    restore_flags(flags);

    switch (return_code) {
	case 0:
	    return address + entry;
	case 0x80:      /* Not present */
	    kprintf("bios32_service(0x%lx) : not present\n", service);
	    return 0;
	default: /* Shouldn't happen */
	    kprintf("bios32_service(0x%lx) : returned unexpected value 0x%x\n",
		    service, return_code);
	    return 0;
    }
}


/*------------------------------------------------------------------------
 * find_pci_device - find a specified PCI device
 *------------------------------------------------------------------------
 */
SYSCALL find_pci_device(int deviceID, int vendorID, int index)
{
    unsigned long retval, info, error;
    int busNum, deviceNum, functionNum;

    /*
     * Returns the location of a PCI device based on the Device ID
     * and Vendor ID.
     */
#ifdef DEBUG
    kprintf("find_pci_device: vendor=0x%x device=0x%x index=%d\n",
	    vendorID, deviceID, index);
#endif
    /*
     * Entry
     * -----
     * AH  - PCI_FUNCTION_CODE
     * AL  - FIND_PCI_DEVICE
     * CX  - Device ID
     * DX  - Vendor ID
     * SI  - Index (0..N)
     *
     * Exit
     * ----
     * BH  - Bus Number (0-255)
     * BL  - Dev Num (upper 5 bits), Function Number (lower 3 bits)
     * AH  - Return Code
     * CF  - Completion status: set=error, cleared=success
     */
/*
    save_flags(flags);
    cli();
*/
    asm(    "lcall *(%%edi)\n"	    /* call the function */
#if 0
	    /* Really need to check the carry flag here */
#else
	    "xor %%ecx, %%ecx\n"    /* placeholding */
#endif
	   :"=a" (retval),
	    "=b" (info),
	    "=c" (error)
	   :"0" (PCIBIOS_FIND_PCI_DEVICE),
	    "2" (deviceID),
	    "d" (vendorID),
	    "S" (index),
	    "D" (&pci_indirect));
/*
    restore_flags(flags);
*/
    /* set up the return values */
    busNum	    = 0xff & (info >> 8);
    deviceNum	    = 0x07 & info;	    /* lower 3 bits */
    functionNum	    = 0x1f & (info >> 3);   /* next 5 bits */
#ifdef DEBUG
    kprintf("find_pci_device: error=0x%x bus=%d dev=0x%x fun=0x%x (info=0x%x)\n",
	    error, busNum, deviceNum, functionNum, info);
    kprintf("retval = 0x%lx\n",0xff & (retval>>8));
#endif
    if (PCIBIOS_SUCCESSFUL == (0xff & (retval>>8))) {
	return info;
    } else
	return SYSERR;

}


/*------------------------------------------------------------------------
 * find_pci_class - find a type of device based on a class code
 *------------------------------------------------------------------------
 */
SYSCALL find_pci_class(unsigned long class_code, unsigned long index) {
    unsigned long   flags;
    unsigned long   retval, id, error;

    kprintf("find_pci_class: code=0x%lx index=%lu\n",class_code, index);
    save_flags(flags);
    cli();
    /* 
     * Entry
     * -----
     * AH  - PCI_FUNCTION_CODE
     * AL  - FIND_PCI_CLASS_CODE
     * ECX - Class Code (lower 3 bytes)
     * SI  - Index (0..N)
     *
     * Exit
     * ----
     * BH  - Bus Number (0-255)
     * BL  - Dev Num (upper 5 bits), Function Number (lower 3 bits)
     * AH  - Return Code
     * CF  - Completion status: set=error, cleared=success
     */
    asm(    "lcall *(%%edi)\n"
#if 0
	    "jc	1f\n"
	    "mov %%ah, %%cl\n"	    /* extract return value */
	    "xor %%eax, %%eax\n"
	    "mov %%cl, %%al\n"
	    "mov $0, %%ecx\n"
	    "jmp 2f\n"
	    "1:\tmov $1, %%ecx\n"
	    "2:\tnop\n"
#endif
	   :"=a" (retval),
	    "=b" (id),
	    "=c" (error)
	   : "0" (PCIBIOS_FIND_PCI_CLASS_CODE),
	     "2" (class_code), 
	     "S" (index),
	     "D" (&pci_indirect));
    restore_flags(flags);
    kprintf("retval: 0x%08lx\n",retval);
    kprintf("    id: 0x%08lx\n",id);
    kprintf(" error: 0x%08lx\n",error);
    return(OK);
}


/*------------------------------------------------------------------------
 * scan_pci_bios - 
 *------------------------------------------------------------------------
 */
int scan_pci_bios(void) {
    unsigned long   vendor, device, index;
    unsigned long   retval, results, error;
    int v;
    unsigned long vtable[3]={0x8086,0xa727,0x10b7}; 
/*
    unsigned long vtable[1]={0x10b7};
*/

#if 0
    for (vendor=0; vendor<PCIBIOS_MAX_VENDOR; vendor++) {
#else
    for (v=0; v<3; v++) {
	vendor=vtable[v];
#endif
	for (device=0; device<PCIBIOS_MAX_DEVICE; device++) {
	    for (index=0; index<PCIBIOS_MAX_INDEX; index++) {
		/*
		kprintf("DEBUG vendor=0x%lx device=0x%lX index=%d\n",vendor,device,index);
		*/
		asm(    "lcall *(%%edi)\n"
			:   "=a" (retval),
			"=b" (results),
			"=c" (error)
			:   "0"  (PCIBIOS_FIND_PCI_DEVICE),
			"2"  (device),
			"d"  (vendor),
			"S"  (index),
			"D"  (&pci_indirect));
		switch (retval>>8) {
		    case PCIBIOS_SUCCESSFUL:
			kprintf("vendor=0x%lx device=0x%lX index=%d devfn=%d\n",vendor,device,index,results);
			break;
		    case PCIBIOS_DEVICE_NOT_FOUND:
			index=PCIBIOS_MAX_INDEX;
			break; /* should stop this round of index */
		    default:
			kprintf("ERROR: vendor=0x%lx device=0x%lX index=%d\n",vendor,device,index);
			panic("Unknown device\n");
		} /* switch */
	    } /* for index */
	} /* for device */
    } /* for vendor */
    return(OK);
}

SYSCALL pci_bios_read_config_byte(unsigned long dev, int where, unsigned char *value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_BYTE),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

SYSCALL pci_bios_read_config_word(unsigned long dev, int where, unsigned short *value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_WORD),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

SYSCALL pci_bios_read_config_dword(unsigned long dev, int where, unsigned long *value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=c" (*value),
		  "=a" (ret)
		: "1" (PCIBIOS_READ_CONFIG_DWORD),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

SYSCALL pci_bios_write_config_byte(unsigned long dev, int where, unsigned char value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (PCIBIOS_WRITE_CONFIG_BYTE),
		  "c" (value),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

SYSCALL pci_bios_write_config_word(unsigned long dev, int where, unsigned short value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (PCIBIOS_WRITE_CONFIG_WORD),
		  "c" (value),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}

SYSCALL pci_bios_write_config_dword(unsigned long dev, int where, unsigned long value)
{
	unsigned long ret;

	__asm__("lcall *(%%esi)\n\t"
		"cld\n\t"
		"jc 1f\n\t"
		"xorb %%ah, %%ah\n"
		"1:"
		: "=a" (ret)
		: "0" (PCIBIOS_WRITE_CONFIG_DWORD),
		  "c" (value),
		  "b" (dev),
		  "D" ((long) where),
		  "S" (&pci_indirect));
	return (int) (ret & 0xff00) >> 8;
}
