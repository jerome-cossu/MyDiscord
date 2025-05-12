// client.h
#ifndef CLIENT_H
#define CLIENT_H

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void send_message_to_server(const char *message);
void on_send_button_clicked(GtkButton *button, gpointer user_data);
void on_receive_button_clicked(GtkButton *button, gpointer user_data);
void on_close_button_clicked(GtkButton *button, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);

#endif // CLIENT_H
