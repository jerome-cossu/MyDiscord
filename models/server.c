#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <libpq-fe.h>

#define PORT 8080

// Déclare les fonctions de la base de données
PGconn *connect_db();
int insert_user(PGconn *conn, const char *last_name, const char *first_name, const char *nickname, const char *email, const char *password);
int verify_user(PGconn *conn, const char *email, const char *password);
void close_db(PGconn *conn);

void handle_client(int client_socket) {
    char buffer[1024];
    int n;

    // Lire les données envoyées par le client
    n = read(client_socket, buffer, sizeof(buffer));
    buffer[n] = '\0';

    // Connexion à la base de données
    PGconn *conn = connect_db();

    if (strcmp(buffer, "signup") == 0) {
        // Inscription : attend des données utilisateur et crée un compte
        char last_name[100], first_name[100], nickname[100], email[100], password[100];
        read(client_socket, last_name, sizeof(last_name));
        read(client_socket, first_name, sizeof(first_name));
        read(client_socket, nickname, sizeof(nickname));
        read(client_socket, email, sizeof(email));
        read(client_socket, password, sizeof(password));

        if (insert_user(conn, last_name, first_name, nickname, email, password)) {
            write(client_socket, "Inscription réussie", 20);
        } else {
            write(client_socket, "Erreur d'inscription", 21);
        }
    }
    else if (strcmp(buffer, "login") == 0) {
        // Connexion : vérifie les identifiants
        char email[100], password[100];
        read(client_socket, email, sizeof(email));
        read(client_socket, password, sizeof(password));

        if (verify_user(conn, email, password)) {
            write(client_socket, "Connexion réussie", 18);
        } else {
            write(client_socket, "Erreur de connexion", 20);
        }
    }

    close_db(conn);
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Crée un socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erreur de socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Lier le socket au port
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de binding");
        exit(1);
    }

    // Écouter les connexions entrantes
    listen(server_socket, 5);

    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

    // Accepter les connexions et traiter les clients
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) >= 0) {
        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
