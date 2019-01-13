#ifndef _PROCESS_H_
#define _PROCESS_H_

#define DATA_SIZE           12
#define SOCK_DATA_SIZE      (DATA_SIZE + 5)
#define SOCK_PREAMBLE       0x7f7f

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

typedef enum
{
    INTERRUPT_SIG,
    CLIENT_TX,
    SERVER_RX = CLIENT_TX,
    CLIENT_TX_RX,
    SERVER_RX_TX = CLIENT_TX_RX,
    DATA_ENUM_MAX,
}sock_data_e;

typedef struct sock_data
{
    uint16_t head;
    sock_data_e flag;
    uint8_t payload[DATA_SIZE];
    uint8_t crc_res;
}sock_data_st;


int parse_sock_rx(int sock_fd, char* buf, int len);

void dump_buf(char* string,char* buf, int len);
void dump_struct(char* string,sock_data_st *st_data);

#endif //_PROCESS_H_