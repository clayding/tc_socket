#ifndef _CLIENT_H_
#define _CLIENT_H_


int client_init(void);

void client_exit(void);

int client_write(int sock_fd,char* w_buf, int w_len);

int client_transfer(int sock_fd,char* w_buf, char* r_buf,int rw_len);

void client_int_handle(void);

#endif //_CLIENT_H_