// gui.c
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>    // Remplace sys/socket.h par winsock2.h
#include <ws2tcpip.h>    // Remplace arpa/inet.h par ws2tcpip.h

#define SERVER_ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

static void send_message_to_server(const char *message) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return;
    }

    send(sock, message, strlen(message), 0);
    closesocket(sock);
    WSACleanup();
}

static void on_send_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEditable *entry = GTK_EDITABLE(user_data);
    const char *message = gtk_editable_get_text(entry);
    g_print("Message envoyé : %s\n", message);
    
    send_message_to_server(message);  // Envoyer le message au serveur
    
    gtk_editable_set_text(entry, "");  // Vider le champ de texte
}

static void on_close_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *window = GTK_WINDOW(user_data);
    gtk_window_close(window);
}

void activate_app(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(window), box);

    GtkWidget *label = gtk_label_new("Bienvenue sur MyDiscord !");
    gtk_box_append(GTK_BOX(box), label);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Tapez votre message...");
    gtk_box_append(GTK_BOX(box), entry);

    GtkWidget *send_button = gtk_button_new_with_label("Envoyer");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_button_clicked), entry);
    gtk_box_append(GTK_BOX(box), send_button);

    GtkWidget *close_button = gtk_button_new_with_label("Fermer");
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close_button_clicked), window);
    gtk_box_append(GTK_BOX(box), close_button);

    gtk_window_present(GTK_WINDOW(window));
}
