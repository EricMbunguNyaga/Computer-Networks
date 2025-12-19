#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    char expr[1024], buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // 1. Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    while (1) {
        printf("Enter expression (e.g., sin(30), 10+20, 5/2): ");
        fgets(expr, sizeof(expr), stdin);
        expr[strcspn(expr, "\n")] = 0;

        // Stop client
        if (strcmp(expr, "exit") == 0)
            break;

        // 3. Send expression
        sendto(sockfd, expr, strlen(expr) + 1, 0,
               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

        // 4. Receive result
        addr_size = sizeof(serverAddr);
        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&serverAddr, &addr_size);

        printf("Result: %s\n\n", buffer);
    }

    close(sockfd);
    return 0;
}

