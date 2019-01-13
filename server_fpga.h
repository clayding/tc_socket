#ifndef _SERVER_H_
#define _SERVER_H_

int server_init(void);
void server_exit(void);
/*The data server received from client should write to spi interface */
void server_receive(char* server_rx_buf, int server_rx_buf_len);

//void server_send(char* server_tx_buf, int server_tx_buf_len);

int server_transfer(int sock_fd,char* sock_data, int sock_data_len);

int server_send_interrupt(void);

#endif //_SERVER_H_