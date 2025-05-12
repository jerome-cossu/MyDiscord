#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "gui.h"  // Assure-toi que gui.h contient la déclaration de activate


#define SERVER_ADDR "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

// Fonction pour envoyer le message au serveur
static void send_message_to_server(const char *message) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        perror("WSAStartup failed");
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        perror("Connection failed");
        closesocket(sock);
        WSACleanup();
        return;
    }

    send(sock, message, strlen(message), 0);
    // closesocket(sock);
    // WSACleanup();
}

// Fonction pour démarrer l'application GTK
void start_gui(GtkApplication *app, gpointer user_data) {
    // Appel de la fonction d'affichage depuis gui.c
    activate(app, user_data);  // C'est ici que tu affiches la fenêtre
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Créer l'application GTK
    app = gtk_application_new("com.mycompany.MyDiscord", G_APPLICATION_DEFAULT_FLAGS);

    // Exécuter l'application GTK
    g_signal_connect(app, "activate", G_CALLBACK(start_gui), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}
