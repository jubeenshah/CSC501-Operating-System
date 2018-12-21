#include <./mon/monconf.h>
#include <./mon/monitor.h>
#include <stdio.h>

/* generic priority queue processing functions */
struct	qinfo {
	char	q_valid;
	int	q_max;
	int	q_count;
	int	q_seen;
	int	q_mutex;
	int	*q_key;
	char	**q_elt;
};

#define	MAXNQ 	32	

static	struct	qinfo mon_Q[MAXNQ];

/*------------------------------------------------------------------------
 * mon_enq  --	insert an item at the tail of a list, based on priority
 *	Returns the number of slots available; -1, if full
 *------------------------------------------------------------------------
 */
int mon_enq(int q, char *elt, int key)
/* q   - queue number			*/
/* elt - item to enqueue on a list	*/
/* key - priority			*/
{
    STATWORD	ps;
    struct	qinfo	*qp;
    int	i, j, left;

    if (q < 0 || q >= MAXNQ)
	return -1;
    if (!mon_Q[q].q_valid || mon_Q[q].q_count >= mon_Q[q].q_max)
	return -1;

    qp = &mon_Q[q];

    disable(ps);

    /* start at tail and move towards head, as long as key is greater */

    /* this shouldn't happen, but... */
    if (qp->q_count < 0)
	qp->q_count = 0;
    i = qp->q_count-1;

    while(i >= 0 && key > qp->q_key[i])
	--i;
    /* i can be -1 (new head) -- it still works */

    for (j = qp->q_count-1; j > i; --j) {
	qp->q_key[j+1] = qp->q_key[j];
	qp->q_elt[j+1] = qp->q_elt[j];
    }
    qp->q_key[i+1] = key;
    qp->q_elt[i+1] = elt;
    qp->q_count++;
    left = qp->q_max - qp->q_count;

    restore(ps);
	
    return left;
}


/*------------------------------------------------------------------------
 * mon_deq --  remove an item from the head of a list and return it
 *------------------------------------------------------------------------
 */
char *mon_deq(int q)
{
    struct	qinfo	*qp;
    STATWORD	ps;
    char	*elt;
    int	i;

    if (q < 0 || q >= MAXNQ)
	return NULL;
    
    if (!mon_Q[q].q_valid || mon_Q[q].q_count <= 0)
	return NULL;

    qp = &mon_Q[q];

    disable(ps);

    elt = qp->q_elt[0];

    for (i=1; i<qp->q_count; ++i) {
	qp->q_elt[i-1] = qp->q_elt[i];
	qp->q_key[i-1] = qp->q_key[i];
    }
    qp->q_count--;
    restore(ps);
    return(elt);
}

/*------------------------------------------------------------------------
 *  mon_newq --  allocate a new queue, return the queue's index
 *------------------------------------------------------------------------
 */
int mon_newq(int size)
{
    struct	qinfo	*qp;
    int i;

    for (i=0; i<MAXNQ; ++i) {
	if (!mon_Q[i].q_valid)
	    break;
    }
    if (i == MAXNQ)
	return -1;
    qp = &mon_Q[i];
    qp->q_valid = TRUE;
    qp->q_max = size;
    qp->q_count = 0;
    qp->q_seen = -1;
    qp->q_elt = (char **) getmem(sizeof(char *) * size);
    qp->q_key = (int *) getmem(sizeof(int) * size);
    if (qp->q_key == (int *) SYSERR || qp->q_elt == (char **) SYSERR)
	return -1;
    return i;
}

/*-------------------------------------------------------------------------
 * mon_lenq - 
 *-------------------------------------------------------------------------
 */
int mon_lenq(int q)
{
    if (q < 0 || q >= MAXNQ || !mon_Q[q].q_valid)
	return SYSERR;
    return mon_Q[q].q_count;
}


/*-------------------------------------------------------------------------
 * mon_initq - 
 *-------------------------------------------------------------------------
 */
int mon_initq()
{
    int i;

    for (i=0; i<MAXNQ; ++i)
	mon_Q[i].q_valid = FALSE;
    return(OK);
}

/*------------------------------------------------------------------------
 * mon_headq -- return the first element in the queue without removing it
 *------------------------------------------------------------------------
 */
char * mon_headq(int q)
{
        struct  qinfo   *qp;
        STATWORD        ps;
        char            *elt;

        if (q < 0 || q >= MAXNQ)
                return NULL;
        if (!mon_Q[q].q_valid || mon_Q[q].q_count == 0)
                return NULL;

        qp = &mon_Q[q];

        disable(ps);
        elt = qp->q_elt[0];
        restore(ps);
        return(elt);
}

