#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

int main() {
    int sockfd;
    struct sockaddr_in dest;
    char packet[1024];

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if(sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    // Destination IP
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("10.0.0.2"); // target IP

    // ICMP header inside packet
    struct icmphdr *icmp_hdr = (struct icmphdr *) packet;
    icmp_hdr->type = 13;          // Timestamp request
    icmp_hdr->code = 0;
    icmp_hdr->checksum = 0;
    icmp_hdr->un.echo.id = htons(1234);
    icmp_hdr->un.echo.sequence = htons(1);

    // TODO: calculate checksum here

    // Send packet
    if(sendto(sockfd, packet, sizeof(struct icmphdr), 0, 
              (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Send failed");
    } else {
        printf("ICMP Timestamp packet sent!\n");
    }

    close(sockfd);
    return 0;
}

