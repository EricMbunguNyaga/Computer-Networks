#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <math.h>

double compute(char *expr) {
    double a, b;

    // sin(x)
    if (sscanf(expr, "sin(%lf)", &a) == 1)
        return sin(a);

    // cos(x)
    if (sscanf(expr, "cos(%lf)", &a) == 1)
        return cos(a);

    // tan(x)
    if (sscanf(expr, "tan(%lf)", &a) == 1)
        return tan(a);

    // sqrt(x)
    if (sscanf(expr, "sqrt(%lf)", &a) == 1)
        return sqrt(a);

    // inverse (1/x)
    if (sscanf(expr, "inv(%lf)", &a) == 1)
        return 1.0 / a;

    // Addition: x + y
    if (sscanf(expr, "%lf+%lf", &a, &b) == 2)
        return a + b;

    // Subtraction: x - y
    if (sscanf(expr, "%lf-%lf", &a, &b) == 2)
        return a - b;

    // Multiplication: x * y
    if (sscanf(expr, "%lf*%lf", &a, &b) == 2)
        return a * b;

    // Division: x / y
    if (sscanf(expr, "%lf/%lf", &a, &b) == 2)
        return a / b;

    // Unknown expression
    return NAN;
}

int main() {
    int sockfd;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // 1. Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // 2. Define server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind socket
    bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    printf("UDP Scientific Calculator Server Running...\n");

    while (1) {
        addr_size = sizeof(clientAddr);

        // 4. Receive expression from client
        recvfrom(sockfd, buffer, sizeof(buffer), 0,
                 (struct sockaddr*)&clientAddr, &addr_size);

        printf("Client expression: %s\n", buffer);

        // 5. Compute result
        double result = compute(buffer);

        // 6. Prepare reply
        char reply[64];
        if (isnan(result))
            sprintf(reply, "Invalid Expression");
        else
            sprintf(reply, "%lf", result);

        // 7. Send result back
        sendto(sockfd, reply, strlen(reply) + 1, 0,
               (struct sockaddr*)&clientAddr, addr_size);
    }

    close(sockfd);
    return 0;
}

