#ifndef __TC_SPI_H
#define __TC_SPI_H

#include <signal.h>

#define FPGA_REG_VERSION		0x00
#define FPGA_REG_SOFT_RESET		0x01
#define FPGA_REG_START			0x02
#define FPGA_REG_ABORT			0x00
#define FPGA_REG_NONCE_IN		0x00
#define FPGA_REG_NONCE_NUM		0x00
#define FPGA_REG_HEADER_3		0x00
#define FPGA_REG_HEADER_2		0x00
#define FPGA_REG_HEADER_1		0x00
#define FPGA_REG_HEADER_0		0x00
#define FPGA_REG_TARGET_1		0x00
#define FPGA_REG_TARGET_0		0x00
#define FPGA_REG_FRUITTARGET_1	0x00
#define FPGA_REG_FRUITTARGET_0	0x00


#define FPGA_REG_NONCE_OUT	0x00
#define FPGA_REG_DIGEST_3		0x00
#define FPGA_REG_DIGEST_2		0x00
#define FPGA_REG_DIGEST_1		0x00
#define FPGA_REG_DIGEST_0		0x00

#define FPGA_REG_INT_STATE		0x00
#define FPGA_REG_INT_IRR		0x00
#define FPGA_REG_SYS_STATE	0x00


#define     SPI_SLAVE_NUM   4

int tc_spi_open(void);
void tc_spi_close(void);
void tc_spi_send(char* tx_buf, int tx_buf_len);
int tc_spi_transfer(char* tx_buf, int tx_buf_len,
	char* rx_buf, int rx_buf_len);

#endif
