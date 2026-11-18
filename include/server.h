#ifndef SERVER_H
#define SERVER_H
int server_init(char *run_port);
int server_listen(int *file_descriptors, int max_listeners);
int server_accept_conn();
int server_shutdown(void);
#endif