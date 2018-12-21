#ifndef _MONEEPRO_H_
#define _MONEEPRO_H_

/* eepro.h - Intel EtherExpress PRO/10 */

#define EEP_RCV_DONE		0x0008
#define EEP_RX_OK		0x2000
#define EEP_RX_ERROR		0x0d81

#define EEP_TX_OK		0x2000
#define EEP_TX_MAX_COLLISIONS	0x0020
#define EEP_TX_NO_CARRIER	0x0400
#define EEP_TX_DMA_UNDERRUN	0x0100
#define EEP_TX_DEFFERED		0x0080
#define EEP_TX_COLLISION_MASK	0x000f

/*
 * TX block definitions
 */
#define I82595_TX_DONE_BIT	0x0080
#define I82595_CHAIN_BIT	0x8000		/* 1st bit of the count word */
/*
 * offsets
 */
#define I82595_TX_STATUS_OFF	0x02		/* offset to status word */
#define I82595_TX_CHAIN_OFF	0x04		/* offset to chain word */
#define I82595_TX_COUNT_OFF	0x06		/* offset to count word	*/

/* Bank 0 registers */
#define EEP_COMMAND_REG			0x00    /* Register 0 */
#define		EEP_CMD_STATUS_MASK	0x0f
#define 	EEP_MC_SETUP		0x03
#define 	EEP_XMT_CMD		0x04
#define 	EEP_DIAGNOSE_CMD	0x07
#define 	EEP_RCV_ENABLE_CMD	0x08
#define 	EEP_RCV_DISABLE_CMD	0x0a
#define 	EEP_STOP_RCV_CMD	0x0b
#define 	EEP_RESET_CMD		0x0e
#define 	EEP_POWER_DOWN_CMD	0x18
#define 	EEP_RESUME_XMT_CMD	0x1c
#define 	EEP_SEL_RESET_CMD	0x1e
#define 	EEP_BANK0_SELECT	0x00
#define 	EEP_BANK1_SELECT	0x40
#define 	EEP_BANK2_SELECT	0x80

#define EEP_STATUS_REG			0x01    /* Register 1 */
#define 	EEP_RX_STP_INT		0x01
#define 	EEP_RX_INT		0x02
#define 	EEP_TX_INT		0x04
#define		EEP_EXEC_INT		0x08
#define		EEP_EXEC_ACTIVE		0x20
#define 	EEP_EXEC_ABORT		0x21
#define 	EEP_RCV_RDY		0x40
#define		EEP_RCV_ACTIVE		0x80
#define		EEP_STOP_IN_PROG	0xc0

#define EEP_ID_REG			0x02    /* Register 2   */
#define 	EEP_R_ROBIN_BITS	0xc0    /* round robin counter */
#define 	EEP_ID_REG_MASK		0xec
#define 	EEP_ID_REG_SIG		0x24
#define 	EEP_AUTO_ENABLE		0x10

#define EEP_INT_MASK_REG		0x03    /* Register 3   */
#define 	EEP_RX_STOP_MASK	0x01
#define 	EEP_RX_MASK		0x02
#define 	EEP_TX_MASK		0x04
#define 	EEP_EXEC_MASK		0x08
#define 	EEP_ALL_MASK		0x0f
/* The following are word (16-bit) registers */
#define EEP_RCV_BAR			0x04
#define EEP_RCV_STOP			0x06
#define EEP_XMT_BAR			0x0a
#define EEP_HOST_ADDRESS_REG		0x0c
#define EEP_IO_PORT			0x0e

/* Bank 1 registers */
#define EEP_REG1			0x01
#define		EEP_WORD_WIDTH			0x02
#define 	EEP_ALT_RDY_TM			0x40
#define		EEP_TRI_ST_INT			0x80
#define	EEP_INT_SEL_REG			0x02
#define		EEP_INT_ENABLE			0x80
#define	EEP_RCV_LOWER_LIMIT_REG		0x08
#define EEP_RCV_UPPER_LIMIT_REG		0x09
#define EEP_XMT_LOWER_LIMIT_REG		0x0a
#define EEP_XMT_UPPER_LIMIT_REG		0x0b

/* Bank 2, reg 1 */
#define EEP_XMT_Chain_Int	0x20 	/* Int. at the end of the transmit chain */
#define EEP_XMT_Chain_ErrStop	0x40 	/* Int. at the end of the chain even if there are errors */
#define EEP_RCV_Disc_BadFrame	0x80 	/* discard bad recvd frames */

/* Bank 2 reg2 */
#define EEP_REG2		0x02
#define 	EEP_PRMSC_Mode		0x01
#define		EEP_BC_DISC		0x02
#define		EEP_CRC_MEM		0x04
#define		EEP_MC_ALL		0x08
#define		EEP_NO_SA_INS		0x10
#define 	EEP_MULTI_IA		0x20
#define 	EEP_INT_LOOPBACK	0x40
#define		EEP_EXT_LOOPBACK	0x80

/* Bank 2, reg 3 */
#define EEP_REG3		0x03
#define 	EEP_TPE_BIT		0x04
#define		EEP_AUTO_PORT		0x10		
#define 	EEP_BNC_BIT		0x20
#define		EEP_TEST2		0x40
#define		EEP_TEST1		0x80

#define EEP_I_ADD_REG0		0x04
#define EEP_I_ADD_REG1		0x05
#define EEP_I_ADD_REG2		0x06
#define EEP_I_ADD_REG3		0x07
#define EEP_I_ADD_REG4		0x08
#define EEP_I_ADD_REG5		0x09

#define EEP_EEPROM_REG		0x0a
#define EEP_EESK		0x01
#define EEP_EECS		0x02
#define EEP_EEDI		0x04
#define EEP_EEDO		0x08

/*
 * TX and RX buffers for 32K and 64K on-board RAM
 */
#define EEP_RAM_SIZE_32		0x8000	/* 32K RAM size   */
#define EEP_TX_SIZE_32		0x1800	/* TX buffer size */		
#define EEP_RX_SIZE_32		(EEP_RAM_SIZE_32 - EEP_TX_SIZE_32);

#define EEP_RAM_SIZE_64		0x10000	/* 64K RAM size   */
#define EEP_TX_SIZE_64		0x3000	/* TX buffer size */
#define EEP_RX_SIZE_64		(EEP_RAM_SIZE_64 - EEP_TX_SIZE_64)

#define EEP_RX_HEADER		8	/* TX header size */
#define EEP_TX_HEADER		8	/* RX header size */

/*
 * Macros
 */

#endif
