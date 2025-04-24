#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server, client;
    int c;
    char buffer[BUFFER_SIZE] = {0};

    WSAStartup(MAKEWORD(2,2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&server, sizeof(server));
    listen(server_fd, 1);

    printf("Server listening on port %d...\n", PORT);
    c = sizeof(struct sockaddr_in);
    client_fd = accept(server_fd, (struct sockaddr *)&client, &c);

    recv(client_fd, buffer, BUFFER_SIZE, 0);
    printf("Message from client: %s\n", buffer);

    char *msg = "Hello from server!";
    send(client_fd, msg, strlen(msg), 0);

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();
    return 0;
}