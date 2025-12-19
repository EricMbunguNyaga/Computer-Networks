#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("10.0.0.1"); // IP of server host

    connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    char fruit[20];
    int qty;

    printf("Enter fruit name: ");
    scanf("%s", fruit);

    printf("Enter quantity: ");
    scanf("%d", &qty);

    // Send purchase request
    send(clientSocket, fruit, sizeof(fruit), 0);
    send(clientSocket, &qty, sizeof(qty), 0);

    // Receive server response
    char buffer[500];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("\nServer Response:\n%s\n", buffer);

    recv(clientSocket, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    close(clientSocket);
    return 0;
}

