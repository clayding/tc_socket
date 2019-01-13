#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/in.h>  
#include <sys/types.h>  

#include "server_fpga.h"

#define PORT 7891

int socketfd,accsocfd;

int server_init()
{
    struct sockaddr_in s_addr,r_addr;
    socklen_t len; 
    int recdata;
    char buf[1024];
    memset(buf,0x00,sizeof(buf));
    //创建套接字
    if(-1 == (socketfd = socket(AF_INET,SOCK_STREAM,0))){
        printf("socketfd is created failed!\n");
        return -1;        
    }    
    printf("Server: socket fd created success!\n"); 
     
    //将本地协议地址与sockfd绑定
    memset(&s_addr,0x00,sizeof(s_addr));
    s_addr.sin_family = PF_INET;
    s_addr.sin_port = htons(PORT);
    s_addr.sin_addr.s_addr = htons(INADDR_ANY);//inet_addr_any 一个服务器可能有多个网卡，随便从中选1个
    if(-1 == bind(socketfd,(struct sockaddr*)&s_addr,sizeof(s_addr))){
        printf("bind failed!\n");            
        return -1;
    }
    printf("bind suc!\n");
    
    //监听本地端口
    if(-1 == listen(socketfd,10)){
        printf("listen failed!\n");
        return -1;
    }
    printf("listen suc!\n");
    
    while(1){
        len = sizeof(struct sockaddr);
        accsocfd = accept(socketfd,(struct sockaddr *)&r_addr,&len);
        if(-1 == accsocfd){
            printf("accept failed!\n");
            return -1;
        }
        printf("accept suc !\nServer get connect from %x port is %x",ntohl(r_addr.sin_addr.s_addr),ntohl(r_addr.sin_port));
        
        for(;;){
            /*服务端->客户端 中断信号
            * 7f 7f 00 01 02 03 04 05 06 07 08 09 0a 0b 0c crc1 crc2
            * 客户端->服务端
            * 7f 7f 01 01 02 03 04 05 06 07 08 09 0a 0b 0c crc1 crc2  client -> server
            * 7f 7f 02 01 02 03 04 05 06 07 08 09 0a 0b 0c crc1 crc2  client -> server -> client
            * */
            int recv_len = 0;
            char recv_buf[128] = {0};

            recv_len = recv(accsocfd, recv_buf, sizeof(recv_buf), 0)
            if(-1 == recv_len){
                printf("recv failed!\n");
                return -1;
            }
            
            //解析报文
            parse_sock_data(accsocfd, recv_buf, recv_len);
        }
        
    }
    return 0;
}

void server_exit(void)
{
    if(accsocfd)
        close(accsocfd);
    if(socketfd)
        close(socketfd);
}

static void server_spi_send(char* spi_tx_buf, int spi_tx_buf_len)
{
	spi_txonly(spi_tx_buf, spi_tx_buf_len);
}

static int server_spi_transfer(char* spi_tx_buf, int spi_tx_buf_len,
	char* spi_rx_buf, int spi_rx_buf_len)
{
	return spi_txrx(spi_tx_buf, spi_tx_buf_len, spi_rx_buf, spi_rx_buf_len);
}

/*The data server received from client should write to spi interface */
void server_receive(int sock_fd,char* server_rx_buf, int server_rx_buf_len)
{
    server_spi_send(server_rx_buf, server_rx_buf_len);
}

int server_transfer(int sock_fd,char* sock_data, int sock_data_len)
{
    int server_tx_buf_len = 0;
    char server_tx_buf[128];
    sock_data_st *server_sdata = (sock_data_st*)sock_data;

    server_tx_buf_len = server_spi_transfer( server_sdata.payload, sizeof( server_sdata.payload),
        server_tx_buf,sizeof( server_sdata.payload));
    
    memcpy(server_sdata.payload,server_tx_buf,server_tx_buf_len);

    server_sdata.crc_res = 0x00;

    server_tx_buf_len =  write(sock_fd,"this is first data from sr!\n",sizeof(sock_data_st));
    if(-1 == server_tx_buf_len){
        printf("Server:write failed!\n");
        return -1;
    }

    return server_tx_buf_len;
}

void server_send_interrupt(void)
{
    sock_data_st int_sdata;

    memset(&int_sdata,0,sizeof(sock_data_st));

    int_sdata.head = SOCK_PREAMBLE;
    int_sdata.flag = INTERRUPT_SIG;
    client_sdata.crc_res = 0x00;

    if(-1 == send(sock_fd,&int_sdata,sizeof(sock_data_st),0))
        printf("Server:send interrupt signal failed\n");
        return -1;
    }
}