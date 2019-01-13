#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
//#include <bcm2835.h>
//#include <wiringPi.h>


#include "tc_fpga.h"
#include "process.h"
#include "server_fpga.h"

static unsigned char spi_init_done = 0;        //0: bcm2835 notinitialized 1: bcm2835 initialized
void spi_end_handle(int param)
{
  printf("Note: Ctrl+c pressed!\n");
/*
  if(spi_init_done){
    printf("spi closed\n");  
    bcm2835_spi_end();
    bcm2835_close();
  }
*/
  server_exit();
  exit(0);
}

static int spi_signal_register()
{
  if (signal(SIGINT, spi_end_handle) == SIG_ERR) {  //handle the CTRL+C signal ,when exit those keys pressed
    perror("signal SIGINT");
    return 1;
  }
  return 0;
}

#define   SPI_SLAVE_INT_HANDLE(x)  (spi_slave##x##_isr_handle) 

#define PI_GPIO_ISR_REGISTER(pin,edge_type,function_name) do{ \
              wiringPiISR (pin,edge_type,function_name); \
            }while(0);
static volatile int isr_cnt = 0;
static void spi_slave1_isr_handle(void)
{
	server_send_interrupt();
	printf("####spi slave 1 isr handle####\n");
	fflush (stdout) ;
}

/*
static void gpio_ext_int_init(void)
{
	wiringPiSetup () ;
	//GPIO 0-7 used for external interrupt handler
	PI_GPIO_ISR_REGISTER (0, INT_EDGE_RISING, spi_slave1_isr_handle) ;
}

static int spi_init(void)
{
	if (!bcm2835_init())
	{
		printf("bcm2835_init failed. Are you running as root??\n");
		return 1;
	}

	if (!bcm2835_spi_begin())  {
		printf("bcm2835_spi_begin failed. Are you running as root??\n");
		return 1;
	}

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);   // The default

	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);          // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);          
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);   // the default

	spi_init_done = 1;

	return 0;
}

static void spi_close(void)
{
	bcm2835_spi_end();
	bcm2835_close();
}
*/
int tc_spi_open(void)
{
	if(spi_signal_register() !=0){  //response to CTL C signal to release spi if spi_init_done=1 
		return -1;
	}
/*
	if(spi_init() != 0) { //spi init 
		printf("spi init failed\n");
		return -1;
	}
	gpio_ext_int_init();//external gpio interrupt init
*/
	server_init();

	return 0;
}

void tc_spi_close(void)
{
	printf("spi close!\n");
	//spi_close();
}


int main(void)
{
	tc_spi_open();
	return 0;
}

#define DEBUG_DUMP 1
#if (DEBUG_DUMP == 1)
static void dump_packet(char* buf, int len, char* str, int line)
{
	if (len > 12) {
		printf("Packet length is %d > 12\n", len);
		return;
	}

	printf("Line:%3d - %4s -- %02X %02X - %02X - %02X %02X %02X %02X %02X %02X %02X %02X - %02X\n",
		       	line, str,
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5],
			buf[6], buf[7], buf[8], buf[9], buf[10], buf[11]);

	return;
}
#define DUMP_TX_PACKET(buf, len) dump_packet(buf, len, "tx:", __LINE__)
#define DUMP_RX_PACKET(buf, len) dump_packet(buf, len, "rx:", __LINE__)
#else
#define DUMP_TX_PACKET(buf, len)
#define DUMP_RX_PACKET(buf, len)
#endif

static int spi_txonly(char* tx_buf, int tx_buf_len)
{
	DUMP_TX_PACKET(tx_buf, tx_buf_len);
	//bcm2835_spi_writenb(tx_buf, tx_buf_len);

	return 0;
}

#define TX_BUF_LEN 128
static int spi_rxonly(char* rx_buf, int rx_buf_len)
{
	char tx_buf[TX_BUF_LEN];

	if (rx_buf_len > TX_BUF_LEN) {
		rx_buf_len = TX_BUF_LEN;
	}
	
	memset(tx_buf, 0xff, rx_buf_len);

	DUMP_TX_PACKET(tx_buf, rx_buf_len);
	//bcm2835_spi_transfernb(tx_buf, rx_buf, rx_buf_len);
	DUMP_RX_PACKET(rx_buf, rx_buf_len);
	
	return rx_buf_len;
}

static int spi_txrx(char* tx_buf, int tx_buf_len,
	char* rx_buf, int rx_buf_len)
{
	if (rx_buf_len < tx_buf_len) {
		printf("SPI rx buf len is less then tx buf len, which is incorrect!\n");
		return -1;
	}
	
	DUMP_TX_PACKET(tx_buf, tx_buf_len);
	//bcm2835_spi_transfernb(tx_buf, rx_buf, tx_buf_len);
	isr_cnt = 0;
	DUMP_RX_PACKET(rx_buf, rx_buf_len);

	return rx_buf_len;
}

void tc_spi_send(char* tx_buf, int tx_buf_len)
{
	spi_txonly(tx_buf, tx_buf_len);
}

int tc_spi_transfer(char* tx_buf, int tx_buf_len,
	char* rx_buf, int rx_buf_len)
{
	return spi_txrx(tx_buf, tx_buf_len, rx_buf, rx_buf_len);
}

void tc_spi_receive(char* rx_buf, int rx_buf_len)
{
	spi_rxonly(rx_buf, rx_buf_len);
}


