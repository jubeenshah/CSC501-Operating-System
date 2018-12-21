/* dump32.c - dump32 */

#include <conf.h>
#include <kernel.h>

/*-------------------------------------------------------------------------
 * dump32 - dump 32-bit long value
 *-------------------------------------------------------------------------
 */

void dump32(unsigned long n) {

  int i;

  for(i = 31; i>=0; i--) {
    kprintf("%02d ",i);
  }

  kprintf("\n");
  
  for(i=31;i>=0;i--) 
    kprintf("%d  ", (n&(1<<i)) >> i);
    
  kprintf("\n");
}
