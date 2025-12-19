#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

#define PCKT_LEN 8192

int main() {
    int sockfd;
    char buffer[PCKT_LEN];

    struct iphdr *ip = (struct iphdr *) buffer;
    struct tcphdr *tcp = (struct tcphdr *) (buffer + sizeof(struct iphdr));

    struct sockaddr_in dest;

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(sockfd < 0) { perror("Socket error"); exit(1); }

    // Fill IP header fields
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen("ROLL123");
    ip->id = htons(54321);
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr("192.168.1.10"); // source IP
    ip->daddr = inet_addr("192.168.1.20"); // target IP

    // Fill TCP header
    tcp->source = htons(1234);
    tcp->dest = htons(80);
    tcp->seq = 0;
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->syn = 1;
    tcp->window = htons(5840);

    // Copy payload (roll number)
    char *data = buffer + sizeof(struct iphdr) + sizeof(struct tcphdr);
    strcpy(data, "ROLL123");

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr("192.168.1.20");

    // Send packet
    if(sendto(sockfd, buffer, ip->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0)
        perror("Send failed");
    else
        printf("TCP packet sent!\n");

    close(sockfd);
    return 0;
}

