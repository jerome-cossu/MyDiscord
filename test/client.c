#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345

SOCKET sock;

void init_socket() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);  // Initialiser Winsock

    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(PORT);

    connect(sock, (struct sockaddr *)&server, sizeof(server));
}

void send_message_to_server(const char *message) {
    send(sock, message, strlen(message), 0);
}

void cleanup_socket() {
    closesocket(sock);
    WSACleanup();
}
