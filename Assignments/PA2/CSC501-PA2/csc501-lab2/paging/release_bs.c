#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

#define SETONE  1
#define SETZERO 0
#define TWOTEN  1024

SYSCALL release_bs(bsd_t bs_id) {

  /* release the backing store with ID bs_id */
    //kprintf("RELEASE_BSTo be implemented!\n");
    STATWORD ps;
    disable(ps);
    int index = bs_id;
    bsm_tab[index].bs_pid[currpid]  = SETZERO;
    bsm_tab[index].bs_vpno[currpid] = TWOTEN * 4;

    int checkMapping = bsm_tab[index].bs_mapping;
    if (checkMapping == SETZERO) {
      /* code */
      bsm_tab[index].bs_status = 0;
      bsm_tab[index].bs_sem    = SETZERO;
      bsm_tab[index].bs_npages = SETZERO;
      bsm_tab[index].bs_private= SETZERO;
    }

   restore(ps);
   return OK;

}
