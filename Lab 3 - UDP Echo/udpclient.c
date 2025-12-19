#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // 1. Create UDP socket
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Configure server address (IP of UDP server)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    // 3. Send "Hi" to server
    sendto(clientSocket, "Hi", 3, 0,
           (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // 4. Receive reply from server
    addr_size = sizeof(serverAddr);
    recvfrom(clientSocket, buffer, sizeof(buffer), 0,
             (struct sockaddr*)&serverAddr, &addr_size);

    printf("Server replied: %s\n", buffer);

    // 5. Close socket
    close(clientSocket);
    return 0;
}

