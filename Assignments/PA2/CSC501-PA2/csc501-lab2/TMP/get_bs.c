#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

#define SETONE  1
#define SETZERO 0
int get_bs(bsd_t bs_id, unsigned int npages) {

STATWORD ps;
disable(ps);
  /* requests a new mapping of npages with ID map_id */
    //kprintf("get_bs\n");
    int checkSemValue     = bsm_tab[bs_id].bs_sem;
    int checkPrivateValue = bsm_tab[bs_id].bs_private;
    int checkStatus       = bsm_tab[bs_id].bs_status;
    int pagesValue        = bsm_tab[bs_id].bs_npages;
    if (bs_id >= 8 || bs_id < SETZERO || npages > 256 ||
          npages <= SETZERO || checkSemValue == SETONE) {
      /* code */
      restore(ps);
      return SYSERR;
    }

    if (checkPrivateValue == SETONE && checkStatus == 1) {
      /* code */
      restore(ps);
      return SYSERR;
    }

    if (checkPrivateValue == SETZERO && checkStatus == 1) {
      /* code */
      restore(ps);
      pagesValue = bsm_tab[bs_id].bs_npages;
      return pagesValue;
    }

    pagesValue = npages;
    bsm_tab[bs_id].bs_npages = pagesValue;

    bsm_tab[bs_id].bs_pid[currpid] = SETONE;


    restore(ps);
    return npages;

}
