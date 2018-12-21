#ifndef _MONTFTP_H_
#define _MONTFTP_H_

/*
 * tftp.h -- ref: RFC-1350
 */
#define TFTP_TRY	          5
#define TFTP_BLOCK_SIZE		512		/* 512 octets	*/
#define TFTP_DATA_HDR_SZ	4		/* 4 octets	*/
#define TFTP_ACK_SZ		4		/* 4 octets	*/

#define TCTP_TYPE_RRQ		1
#define TCTP_TYPE_WRQ		2
#define TFTP_TYPE_DATA		3
#define TFTP_TYPE_ACK		4
#define TFTP_TYPE_ERROR		5

#define TFTP_INIT_TID		69
#define TFTP_MY_TID		62316

struct tftp_req {
    short type;
    char data[512];
};

struct tftp_data {
    short type;
    short block;
    char data[512];
};

struct tftp_ack {
    short type;
    short block;
};

#endif
