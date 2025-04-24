// server.c
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Initialisation de Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // Créer le socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Préparer l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Lier le socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Écouter les connexions entrantes
    listen(server_fd, 3);
    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Accepter une connexion
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connecté: %s\n", inet_ntoa(client_addr.sin_addr));

    // Lire les messages du client et les renvoyer au même client
    int recv_size;
    while ((recv_size = recv(client_fd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[recv_size] = '\0';  // Assurer la terminaison du message
        printf("Message reçu : %s\n", buffer);

        // Renvoi du message au même client
        send(client_fd, buffer, strlen(buffer), 0);
    }

    // Fermer la connexion du client
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
