#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // 1. Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind socket to IP and port
    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    printf("UDP Server running...\n");

    // 4. Receive message from any client
    addr_size = sizeof(clientAddr);
    recvfrom(sockfd, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&clientAddr, &addr_size);
    printf("Client says: %s\n", buffer);

    // 5. Send reply to the client
    sendto(sockfd, "Hello", 6, 0,
           (struct sockaddr*)&clientAddr, addr_size);

    // 6. Close socket
    close(sockfd);
    return 0;
}

