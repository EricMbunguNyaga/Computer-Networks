#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <linux/if_ether.h>

int main() {
    int sockfd;
    char buffer[65536];

    // 1. Create a RAW socket to capture TCP packets
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
        perror("Socket error");
        return -1;
    }

    printf("TCP Traffic Analyzer Running...\n");

    while (1) {
        // 2. Receive one packet
        int data_size = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (data_size < 0) {
            perror("Recvfrom error");
            return -1;
        }

        // 3. Extract Ethernet header
        struct ethhdr *eth = (struct ethhdr *)buffer;

        printf("\n---- Ethernet Header ----\n");
        printf("Source MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
               eth->h_source[0], eth->h_source[1], eth->h_source[2],
               eth->h_source[3], eth->h_source[4], eth->h_source[5]);
        printf("Dest MAC:   %02X:%02X:%02X:%02X:%02X:%02X\n",
               eth->h_dest[0], eth->h_dest[1], eth->h_dest[2],
               eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

        // 4. Extract IP header
        struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

        struct in_addr src_ip, dst_ip;
        src_ip.s_addr = ip->saddr;
        dst_ip.s_addr = ip->daddr;

        printf("\n---- IP Header ----\n");
        printf("Source IP:      %s\n", inet_ntoa(src_ip));
        printf("Destination IP: %s\n", inet_ntoa(dst_ip));
        printf("Protocol:       %d\n", ip->protocol);
        printf("Header Length:  %d bytes\n", ip->ihl * 4);

        // Process only TCP packets (protocol = 6)
        if (ip->protocol == 6) {

            // 5. Extract TCP header
            int ip_header_len = ip->ihl * 4;
            struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + ip_header_len);

            printf("\n---- TCP Header ----\n");
            printf("Source Port:      %u\n", ntohs(tcp->source));
            printf("Destination Port: %u\n", ntohs(tcp->dest));
            printf("Sequence Number:  %u\n", ntohl(tcp->seq));
            printf("Ack Number:       %u\n", ntohl(tcp->ack_seq));
            printf("Flags:  SYN=%d ACK=%d FIN=%d RST=%d PSH=%d URG=%d\n",
                   tcp->syn, tcp->ack, tcp->fin, tcp->rst, tcp->psh, tcp->urg);

            // 6. Show payload length
            int tcp_header_len = tcp->doff * 4;
            int payload_offset = sizeof(struct ethhdr) + ip_header_len + tcp_header_len;
            int payload_size = data_size - payload_offset;

            printf("TCP Payload Size: %d bytes\n", payload_size);
        }

        printf("\n==============================\n");
    }

    close(sockfd);
    return 0;
}

