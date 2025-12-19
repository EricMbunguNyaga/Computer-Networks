#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // 1. Create TCP socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure server address (IP of server host)
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1");

    // 3. Connect to server
    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // 4. Send “Hi” to server
    send(clientSocket, "Hi", 3, 0);

    // 5. Receive reply from server
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("Server replied: %s\n", buffer);

    // 6. Close socket
    close(clientSocket);
    return 0;
}

