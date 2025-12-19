// Filename: server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Broadcast message to all clients
void broadcast(char *message, int sender_fd) {
    pthread_mutex_lock(&clients_mutex);
    for(int i = 0; i < client_count; i++) {
        if(clients[i] != sender_fd) { // do not send to sender
            send(clients[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

// Save message to log.txt with timestamp
void save_log(char *message) {
    FILE *fp = fopen("log.txt", "a");
    if(!fp) return;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);  // e.g., "Wed Dec 4 22:00:00 2025\n"
    timestamp[strlen(timestamp)-1] = '\0'; // remove newline

    fprintf(fp, "[%s] %s\n", timestamp, message);
    fclose(fp);
}

// Handle client connection
void *client_handler(void *arg) {
    int client_fd = *((int *)arg);
    char buffer[BUFFER_SIZE];

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if(n <= 0) {
            printf("Client disconnected\n");

            // Remove client from clients array
            pthread_mutex_lock(&clients_mutex);
            for(int i=0; i<client_count; i++) {
                if(clients[i] == client_fd) {
                    for(int j=i; j<client_count-1; j++)
                        clients[j] = clients[j+1];
                    client_count--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            close(client_fd);
            break;
        }

        printf("Received: %s\n", buffer);

        save_log(buffer);           // save message to log
        broadcast(buffer, client_fd); // send to all other clients
    }
    return NULL;
}

int main() {
    int sockfd, newfd;
    struct sockaddr_in server, client;
    socklen_t c = sizeof(struct sockaddr_in);
    pthread_t thread_id;

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

    while( (newfd = accept(sockfd, (struct sockaddr*)&client, &c)) ) {
        printf("Accepted connection from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        pthread_mutex_lock(&clients_mutex);
        clients[client_count++] = newfd;
        pthread_mutex_unlock(&clients_mutex);

        if(pthread_create(&thread_id, NULL, client_handler, (void*)&newfd) < 0) {
            perror("Thread error"); continue;
        }

        pthread_detach(thread_id); // detach thread to free resources automatically
    }

    close(sockfd);
    return 0;
}

