// Filename: tcp_file_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 9000
#define BUFFER_SIZE 1024

int main() {
    int sockfd, newfd;
    struct sockaddr_in server, client;
    socklen_t c = sizeof(client);
    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) { perror("Socket error"); exit(1); }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Bind error"); exit(1);
    }

    if(listen(sockfd, 5) < 0) { perror("Listen error"); exit(1); }

    printf("Server listening on port %d...\n", PORT);

    while((newfd = accept(sockfd, (struct sockaddr*)&client, &c))) {
        printf("Connection accepted from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        memset(buffer, 0, BUFFER_SIZE);
        recv(newfd, buffer, BUFFER_SIZE, 0); // receive command
        printf("Client command: %s\n", buffer);

        if(strcmp(buffer, "download") == 0) {
            FILE *fp = fopen("server_file.txt", "rb"); // file in server directory
            if(!fp) { perror("File open error"); close(newfd); continue; }

            clock_t start = clock();

            while(1) {
                int n = fread(buffer, 1, BUFFER_SIZE, fp);
                if(n <= 0) break;
                send(newfd, buffer, n, 0);
            }
            clock_t end = clock();
            double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
            printf("File sent in %.3f seconds\n", time_taken);

            fclose(fp);
        }
        else if(strcmp(buffer, "upload") == 0) {
            FILE *fp = fopen("server_received.txt", "wb"); // file uploaded from client
            if(!fp) { perror("File create error"); close(newfd); continue; }

            clock_t start = clock();

            while(1) {
                int n = recv(newfd, buffer, BUFFER_SIZE, 0);
                if(n <= 0) break;
                fwrite(buffer, 1, n, fp);
            }
            clock_t end = clock();
            double time_taken = ((double)(end - start))/CLOCKS_PER_SEC;
            printf("File received in %.3f seconds\n", time_taken);

            fclose(fp);
        }

        close(newfd);
        printf("Connection closed.\n");
    }

    close(sockfd);
    return 0;
}

