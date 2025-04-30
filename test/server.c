#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

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
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Serveur en écoute sur le port %d...\n", PORT);

    // Accepter une connexion
    client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd == INVALID_SOCKET) {
        printf("Accept failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connecté.\n");

    // Lire les messages du client
    int recv_size;
    while (1) {
        recv_size = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (recv_size == SOCKET_ERROR) {
            int err_code = WSAGetLastError();
            if (err_code == 10053) {
                // Erreur 10053 : La connexion a été abandonnée
                printf("Erreur de connexion : 10053, la connexion a été fermée par le client.\n");
                break; // Sortir de la boucle, mais ne fermer pas tout de suite
            }
            printf("Error receiving data: %d\n", err_code);
            break; // Sortir de la boucle si une autre erreur survient
        }

        if (recv_size == 0) {
            printf("Le client a fermé la connexion.\n");
            break; // Si la taille reçue est zéro, le client a fermé la connexion
        }

        buffer[recv_size] = '\0'; // Ajouter un caractère nul à la fin de la chaîne
        printf("Message reçu : %s\n", buffer);

        // Réponse au client
        const char *response = "Message reçu !";
        send(client_fd, response, strlen(response), 0);
    }

    // Fermer les sockets
    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
