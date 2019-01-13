#include<sys/socket.h>
#include<string.h>
#include<linux/in.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>

#include "client_fpga.h"

#define SERVER_ADDR "172.19.230.113"
#define PORT        7891

static int csocfd;


int main(void)
{
    int res = 0;
    res = client_init();
    if(res) return -1;

    if(signal(SIGINT, client_exit) == SIG_ERR) {
        perror("signal SIGINT");
        return -1;
    }

    while(1){
        char tx_buf[DATA_SIZE] = {0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c};
        char rx_buf[DATA_SIZE] = {0};
        dump_buf("Client write:",tx_buf,sizeof(tx_buf));
        client_write(csocfd,tx_buf,sizeof(tx_buf));
        dump_buf("Client transfer:",tx_buf,sizeof(tx_buf));
        client_transfer(csocfd,tx_buf,rx_buf,sizeof(tx_buf));
        dump_buf("Client read:",rx_buf,sizeof(rx_buf));
        usleep(1000000);
    }
}

int client_init(void)
{
    struct sockaddr_in mysockaddr;
    //创建套接字
    if(-1 == (csocfd = socket(PF_INET,SOCK_STREAM,0))){
        printf("csocfd failed!\n");
        return -1;
    }
    printf("Client: socket fd created successfully!\n");
    
    //设置服务器的地址
    memset(&mysockaddr,0x00,sizeof(mysockaddr));
    mysockaddr.sin_family = PF_INET;
    mysockaddr.sin_port = htons(PORT);
    inet_pton(AF_INET,SERVER_ADDR,&mysockaddr.sin_addr.s_addr);
//s_add.sin_addr.s_addr= inet_addr("172.19.230.113"); /* ip转换为4字节整形，使用时需要根据服务端ip进行更改 */  

    while(1){
        //wait for connectting to the server
        if(-1 == connect(csocfd,(struct sockaddr*)&mysockaddr,sizeof(mysockaddr))){
            printf("connect failed!\n");
            return -1;
        }
        return 0;
    }
}


void client_exit(void)
{
    if(csocfd)
        close(csocfd);
    exit(0);
}


int client_write(int sock_fd,char* w_buf, int w_len)
{
    int send_len = 0;
    sock_data_st client_sdata;

    memset(&client_sdata,0,sizeof(sock_data_st));

    client_sdata.head = SOCK_PREAMBLE;
    client_sdata.flag = CLIENT_TX;

    if(w_len > sizeof(sock_data_st)){
        printf("Client:the data to write exceeds the packet size :%d\n "
            ,sizeof(sock_data_st));
        w_len = sizeof(sock_data_st);
    }
    
    memcpy(client_sdata.payload,w_buf,w_len);

    client_sdata.crc_res = 0x00;

    send_len = send(sock_fd,&client_sdata,sizeof(sock_data_st),0)
    if(-1 == send_len )
        printf("Client:send failed\n");
        return -1;
    }

    return send_len;
}

int client_transfer(int sock_fd,char* w_buf, char* r_buf,int rw_len)
{
    int read_len = 0;
    sock_data_st client_sdata;

    memset(&client_sdata,0,sizeof(sock_data_st));

    client_sdata.head = SOCK_PREAMBLE;
    client_sdata.flag = CLIENT_TX_RX;

    if(rw_len > sizeof(sock_data_st)){
        printf("Client:the data to write exceeds the packet size :%d\n "
            ,sizeof(sock_data_st));
        rw_len = sizeof(sock_data_st);
    }
    
    memcpy(client_sdata.payload,w_buf,rw_len);

    client_sdata.crc_res = 0x00;

    if(-1 == send(sock_fd,&client_sdata,sizeof(sock_data_st),0))
        printf("Client:send failed\n");
        return -1;
    }

    read_len = read(sock_fd,&client_sdata,sizeof(sock_data_st));

    if(-1 == read_len){
        printf("Client:read failed!\n");
        return -1;
    }
    memcpy(r_buf,client_sdata.payload,rw_len);

    return read_len;
}


void client_int_handle(void)
{
    printf("client interrupt handle\n");
    fflush(stdout);
}