#ifndef CLIENT_H
#define CLIENT_H

void init_socket();
void send_message_to_server(const char *message);
void cleanup_socket();

#endif
