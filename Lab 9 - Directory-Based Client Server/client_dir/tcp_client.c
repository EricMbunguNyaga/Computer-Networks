// Filename: tcp_file_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server;
    char buffer[BUFFER_SIZE];
    char choice[10];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("Socket error"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // change to server IP

    if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connect error"); exit(1);
    }

    printf("Enter command (download/upload): ");
    scanf("%s", choice);

    send(sockfd, choice, strlen(choice), 0);

    if(strcmp(choice, "download") == 0) {
        FILE *fp = fopen("client_received.txt", "wb"); // file saved in client directory
        if(!fp) { perror("File create error"); close(sockfd); exit(1); }

        clock_t start = clock();

        while(1) {
            int n = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if(n <= 0) break;
            fwrite(buffer, 1, n, fp);
        }

        clock_t end = clock();
        double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
        printf("File downloaded in %.3f seconds\n", time_taken);

        fclose(fp);
    }
    else if(strcmp(choice, "upload") == 0) {
        FILE *fp = fopen("client_file.txt", "rb"); // file in client directory
        if(!fp) { perror("File open error"); close(sockfd); exit(1); }

        clock_t start = clock();

        while(1) {
            int n = fread(buffer, 1, BUFFER_SIZE, fp);
            if(n <= 0) break;
            send(sockfd, buffer, n, 0);
        }

        clock_t end = clock();
        double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
        printf("File uploaded in %.3f seconds\n", time_taken);

        fclose(fp);
    }

    close(sockfd);
    return 0;
}

