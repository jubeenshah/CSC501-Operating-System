/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

#define SETZERO 0
#define SETONE  1
#define TWOTEN  1024
extern int page_replace_policy;


/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_frm(){
  //kprintf("FRAMETo be implemented!\n");

  STATWORD ps;
  disable(ps);

  int index = SETZERO;
/*typedef struct{
  int fr_status;			/* MAPPED or UNMAPPED		*
  int fr_pid;				/* process id using this frame  *
  int fr_vpno;				/* corresponding virtual page no*
  int fr_refcnt;			/* reference count		*
  int fr_type;				/* FR_DIR, FR_TBL, FR_PAGE	*
  int fr_dirty;
}fr_map_t;
*/
  while (index < TWOTEN) {
    /* code */
    frm_tab[index].fr_status  = 0;
    frm_tab[index].fr_pid     = -SETONE;
    frm_tab[index].fr_vpno    = SETZERO;
    frm_tab[index].fr_refcnt  = SETZERO;
    frm_tab[index].fr_type    = 0;
    frm_tab[index].fr_dirty   = SETZERO;

    scAcc[index]  = SETZERO;
    scPointer   = SETZERO;
    index = index + SETONE;
  }
  restore(ps);
  return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */
SYSCALL get_frm(int* avail){

  //kprintf("%d",page_replace_policy);
  //kprintf("get_frm star!\n");

  STATWORD ps;
  disable(ps);

  int index = SETZERO;
  *avail    = -SETONE;

  int frameNumber;

  while (index < TWOTEN) {
    /* code */
    int checkStatus = frm_tab[index].fr_status;
    //kprintf("Test %d", checkStatus);
    if (checkStatus == 0) {
      //kprintf("IN");
      *avail = index;
      scAcc[index] = SETONE;
      restore(ps);
      return OK;
    }
    index = index + SETONE;
  }
  //kprintf("OUT%d\n",page_replace_policy);
  if (page_replace_policy == 3) { // 3 = Second Chance
    /* code */
    //kprintf();
    //kprintf("replacement policy = SC");
    frameNumber = getFrameSC();
    free_frm(frameNumber);
    scAcc[frameNumber] = SETONE;
    *avail = frameNumber;
    restore(ps);
    return OK;
  }
  //kprintf("get frm end!\n");
  restore(ps);
  return SYSERR;
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{

  //kprintf("To be implemented!\n");
  STATWORD ps;
  disable(ps);
  int pageNumber;
  int index;
  int frameID;
  int storeID;
  int checkType;
  unsigned long virtualAddress;
  unsigned int pageTable;
  unsigned int pageDirectory;
  unsigned long pdbr;
  pd_t *pd_entry;
  pt_t *pt_entry;

  index = i;
  checkType = frm_tab[index].fr_type;

  if (checkType == 0) {
    int v_p_n_o = frm_tab[index].fr_vpno;
    virtualAddress = v_p_n_o;

    int p_i_d = frm_tab[index].fr_pid;
    frameID = p_i_d;

    int p_d_b_r = proctab[frameID].pdbr;
    pdbr = p_d_b_r;

    int andVal = TWOTEN - 1;
    pageTable = virtualAddress & andVal;

    int shiftVal = SETONE * 10;
    pageDirectory = virtualAddress>>shiftVal;

    int proctabStore = proctab[frameID].store;
    storeID = proctabStore;

    int a = sizeof(pd_t);
    int b = pageDirectory;
    int mult = b * a;
    int c = pdbr;
    int add = c + mult;
    pd_entry = add;

    int d = sizeof(pt_t);
    int e = pageTable;
    int multTwo = d * e;
    int twoFourTen = TWOTEN * 4;
    int f = pd_entry->pd_base;
    int multThree = twoFourTen * f;
    int addTwo = multTwo + multThree;
    pt_entry = addTwo;

    int proctabVh = proctab[frameID].vhpno;
    int v_p_n_o_dos = frm_tab[index].fr_vpno;
    pageNumber = v_p_n_o_dos - proctabVh;

    int indexFrame = index + TWOTEN;
    indexFrame = indexFrame * twoFourTen;
    write_bs(indexFrame, storeID, pageNumber);

    pt_entry->pt_pres = SETZERO;
    int frameIndex = f - TWOTEN;

    if ((frm_tab[frameIndex].fr_refcnt - 1) == SETZERO) {
      /* code */
      frm_tab[frameIndex].fr_pid    = -SETONE;
      frm_tab[frameIndex].fr_status = 0;
      frm_tab[frameIndex].fr_vpno   = TWOTEN * 4;
      frm_tab[frameIndex].fr_type   = 0;
    }

   }

   restore(ps);
  return OK;
}

int getFrameSC() {
  STATWORD ps;
  disable(ps);
  int index = SETZERO;
  index = index + scPointer;

  while(SETONE) {
    index = index % TWOTEN;
    int checkType = frm_tab[index].fr_type;
    if (checkType == 0) {
      int checkSCValue = scAcc[index];
      if (checkSCValue == SETONE) {
        int updateSCVAL = SETZERO;
        scAcc[index] = updateSCVAL;
      } else {
        scPointer = index + SETONE;
        restore (ps);
        return index;
      }
    }
    index = index + SETONE;
  }

  restore(ps);
  return SYSERR;
}

void frameDefine(int pid) {
STATWORD ps;
disable(ps);
int index = SETZERO;

while (index < TWOTEN) {
  /* code */
  int checkP_i_d = frm_tab[index].fr_pid;
  if (checkP_i_d != pid) {
    /* code */
    continue;
  }
  frm_tab[index].fr_status 	= 0;
  frm_tab[index].fr_pid		 	= -SETONE;
  frm_tab[index].fr_vpno		= TWOTEN * 4;
  frm_tab[index].fr_refcnt	= SETZERO;
  frm_tab[index].fr_type		= 0;
  frm_tab[index].fr_dirty 	= SETZERO;
  index = index + SETONE;
}
restore(ps);
}
