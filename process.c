#include "process.h"

int parse_sock_rx(int sock_fd, char* buf, int len)
{
    sock_data_st *sdata = NULL;

    if (len != DATA_SIZE) return -1;

    sdata = (sock_data_st*)buf;

    if(sdata->head == SOCK_PREAMBLE && sdata->flag < DATA_ENUM_MAX)
    {
        sdata->crc_res = check_crc16x((unsigned char*)buf,len);
        if (sdata->crc_res) //need to do check_crc16x()
        {
            printf("Crc checked failed\n");
            return 1;
        }		
        switch(sdata->flag)
        {
            case INTERRUPT_SIG:
                client_int_handle();
                break;
            case SERVER_RX:
                /*server write to spi interface*/
                server_receive(sdata->payload,sizeof(sdata->payload));
                break;
            case SERVER_RX_TX:
                /*server write to spi interface and read from spi interface*/
                server_transfer(sock_fd, sdata, sizeof(sock_data_st));
                break;
            default:
                break;
        }
    }

    return 0;
}


void dump_buf(char* string,char* buf, int len)
{
    int i = 0;
    printf("%s",string);
	for(;i < len; i++)
        printf("-%02x",buf[i]);
    printf("n");
}

void dump_struct(char* string,sock_data_st *st_data)
{
   char *ptr = (char*)st_data;
   dump_buf(string,ptr,sizeof(sock_data_st));
}