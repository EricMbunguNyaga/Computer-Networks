#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_FRUITS 5
#define MAX_CUSTOMERS 50

// Fruit structure
struct Fruit {
    char name[20];
    int quantity;
    char last_sold[50];
};

// Customer record <IP, port>
struct Customer {
    char ip[20];
    int port;
};

int main() {
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addr_size;

    // Fruit database
    struct Fruit fruits[MAX_FRUITS] = {
        {"apple", 20, "Never"},
        {"banana", 25, "Never"},
        {"mango", 15, "Never"},
        {"orange", 18, "Never"},
        {"grape", 30, "Never"}
    };

    struct Customer customers[MAX_CUSTOMERS];
    int customer_count = 0;

    // Create TCP socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Server settings
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    listen(serverSocket, 5);
    printf("Fruit Server Running on port 8080...\n");

    while (1) {
        // Accept new client
        addr_size = sizeof(clientAddr);
        newSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addr_size);

        char client_ip[20];
        strcpy(client_ip, inet_ntoa(clientAddr.sin_addr));
        int client_port = ntohs(clientAddr.sin_port);

        // Check if customer is new
        int is_new = 1;
        for (int i = 0; i < customer_count; i++) {
            if (strcmp(customers[i].ip, client_ip) == 0 &&
                customers[i].port == client_port) {
                is_new = 0;
                break;
            }
        }

        if (is_new) {
            strcpy(customers[customer_count].ip, client_ip);
            customers[customer_count].port = client_port;
            customer_count++;
        }

        // Receive purchase request
        char fruit_name[20];
        int req_qty;
        recv(newSocket, fruit_name, sizeof(fruit_name), 0);
        recv(newSocket, &req_qty, sizeof(req_qty), 0);

        int found = 0;

        // Process fruit purchase
        for (int i = 0; i < MAX_FRUITS; i++) {
            if (strcmp(fruits[i].name, fruit_name) == 0) {
                found = 1;

                if (fruits[i].quantity >= req_qty) {
                    fruits[i].quantity -= req_qty;

                    // Record timestamp
                    time_t now = time(NULL);
                    strcpy(fruits[i].last_sold, ctime(&now));

                    char msg[200];
                    sprintf(msg,
                        "Purchase successful!\nRemaining %s: %d\nLast sold: %s",
                        fruits[i].name,
                        fruits[i].quantity,
                        fruits[i].last_sold);

                    send(newSocket, msg, strlen(msg), 0);
                } else {
                    char regret[200];
                    sprintf(regret,
                        "Sorry! Only %d %s left. Cannot complete purchase.",
                        fruits[i].quantity,
                        fruits[i].name);
                    send(newSocket, regret, strlen(regret), 0);
                }
                break;
            }
        }

        if (!found) {
            char err[] = "Fruit not found!";
            send(newSocket, err, strlen(err), 0);
        }

        // Send customer list count
        char count_msg[50];
        sprintf(count_msg, "\nTotal unique customers: %d\n", customer_count);
        send(newSocket, count_msg, strlen(count_msg), 0);

        close(newSocket);
    }

    close(serverSocket);
    return 0;
}

