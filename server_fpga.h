#ifndef _SERVER_H_
#define _SERVER_H_

int server_init(void);
void server_exit(void);
/*The data server received from client should write to spi interface */
void server_receive_from_client(char* server_rx_buf, int server_rx_buf_len);

void server_send_to_client(char* server_tx_buf, int server_tx_buf_len);

int server_transfer_to_client(char* server_rx_buf, int server_rx_buf_len,
	char* server_tx_buf, int server_tx_buf_len);

void server_send_interrupt(void);

#endif //_SERVER_H_