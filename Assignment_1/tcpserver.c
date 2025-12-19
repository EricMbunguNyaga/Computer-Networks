#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;
    char buffer[1024];

    // 1. Create TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind socket to the given IP/Port
    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // 4. Listen for incoming connections
    listen(sockfd, 5);
    printf("Server listening on port 8080...\n");

    // 5. Accept incoming connection
    addr_size = sizeof(clientAddr);
    newsockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &addr_size);

    // 6. Receive message from client
    recv(newsockfd, buffer, sizeof(buffer), 0);
    printf("Client said: %s\n", buffer);

    // 7. Send reply back to client
    send(newsockfd, "Hello", 6, 0);

    // 8. Close sockets
    close(newsockfd);
    close(sockfd);

    return 0;
}

