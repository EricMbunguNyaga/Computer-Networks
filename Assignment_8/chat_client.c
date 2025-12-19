// Filename: client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

void *receive_handler(void *arg) {
    int sockfd = *((int *)arg);
    char buffer[BUFFER_SIZE];

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if(n <= 0) {
            printf("Disconnected from server.\n");
            exit(0);
        }
        printf("%s\n", buffer);
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server;
    pthread_t recv_thread;
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("Socket error"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // change to server IP if needed

    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect error"); exit(1);
    }

    printf("Connected to chat server.\n");

    pthread_create(&recv_thread, NULL, receive_handler, (void*)&sockfd);

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);

        if(strlen(buffer) > 1) {
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }

    close(sockfd);
    return 0;
}

