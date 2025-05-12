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

#define LENGTH 2048

volatile int flag = 0;
char name[32];

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

unsigned __stdcall send_msg_handler(void* sockfd_ptr) {
    SOCKET sockfd = *(SOCKET*)sockfd_ptr;
    char message[LENGTH];

    while (1) {
        str_overwrite_stdout();
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        if (strcmp(message, "exit") == 0) {
            flag = 1;
            break;
        } else {
            send(sockfd, message, (int)strlen(message), 0);
        }
        memset(message, 0, LENGTH);
    }

    _endthreadex(0);
    return 0;
}

unsigned __stdcall recv_msg_handler(void* sockfd_ptr) {
    SOCKET sockfd = *(SOCKET*)sockfd_ptr;
    char message[LENGTH];

    while (1) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            printf("\r%s\n", message);
            str_overwrite_stdout();
        } else if (receive == 0) {
            break;
        }
        memset(message, 0, LENGTH);
    }

    _endthreadex(0);
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return EXIT_FAILURE;
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);

    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation error\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    printf("Enter your name: ");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));
    send(sockfd, name, (int)strlen(name), 0);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    _beginthreadex(NULL, 0, send_msg_handler, &sockfd, 0, NULL);
    _beginthreadex(NULL, 0, recv_msg_handler, &sockfd, 0, NULL);

    while (1) {
        if (flag) {
            printf("\nBye\n");
            break;
        }
    }

    closesocket(sockfd);
    WSACleanup();

    return EXIT_SUCCESS;
}