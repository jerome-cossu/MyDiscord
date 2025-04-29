#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

#pragma comment(lib, "ws2_32.lib")

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048

typedef struct {
    struct sockaddr_in address;
    SOCKET sockfd;
    int uid;
    char name[32];
} client_t;

static client_t* clients[MAX_CLIENTS];
static int uid = 10;
CRITICAL_SECTION clients_mutex;

void str_trim_lf(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void queue_add(client_t* cl) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!clients[i]) {
            clients[i] = cl;
            break;
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

void queue_remove(int uid) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid == uid) {
            clients[i] = NULL;
            break;
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

void send_message(char* s, int uid) {
    EnterCriticalSection(&clients_mutex);
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] && clients[i]->uid != uid) {
            send(clients[i]->sockfd, s, (int)strlen(s), 0);
        }
    }
    LeaveCriticalSection(&clients_mutex);
}

unsigned __stdcall handle_client(void* arg) {
    char buff_out[BUFFER_SZ];
    char name[32];
    int leave_flag = 0;

    client_t* cli = (client_t*)arg;

    // Receive name
    if (recv(cli->sockfd, name, 32, 0) <= 0 || strlen(name) < 2 || strlen(name) >= 31) {
        printf("Didn't enter the name.\n");
        leave_flag = 1;
    }
    else {
        strcpy(cli->name, name);
        sprintf(buff_out, "%s has joined\n", cli->name);
        printf("%s", buff_out);
        send_message(buff_out, cli->uid);
    }

    memset(buff_out, 0, BUFFER_SZ);

    while (!leave_flag) {
        int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
        if (receive > 0) {
            if (strlen(buff_out) > 0) {
                send_message(buff_out, cli->uid);
                str_trim_lf(buff_out, strlen(buff_out));
                printf("%s -> %s\n", buff_out, cli->name);
            }
        }
        else {
            sprintf(buff_out, "%s has left\n", cli->name);
            printf("%s", buff_out);
            send_message(buff_out, cli->uid);
            leave_flag = 1;
        }
        memset(buff_out, 0, BUFFER_SZ);
    }

    closesocket(cli->sockfd);
    queue_remove(cli->uid);
    free(cli);
    _endthreadex(0);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);
    SOCKET listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr, cli_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR: bind failed");
        return EXIT_FAILURE;
    }

    if (listen(listenfd, 10) < 0) {
        perror("ERROR: listen failed");
        return EXIT_FAILURE;
    }

    InitializeCriticalSection(&clients_mutex);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    while (1) {
        int addr_len = sizeof(cli_addr);
        SOCKET connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &addr_len);

        client_t* cli = (client_t*)malloc(sizeof(client_t));
        cli->address = cli_addr;
        cli->sockfd = connfd;
        cli->uid = uid++;

        queue_add(cli);
        uintptr_t tid = _beginthreadex(NULL, 0, handle_client, (void*)cli, 0, NULL);
    }

    DeleteCriticalSection(&clients_mutex);
    closesocket(listenfd);
    WSACleanup();

    return EXIT_SUCCESS;
}
