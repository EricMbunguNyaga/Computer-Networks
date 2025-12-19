#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ether.h>
#include <string.h>

int main() {

    char errbuf[PCAP_ERRBUF_SIZE];

    // Open the saved capture file
    pcap_t *handle = pcap_open_offline("capture.pcap", errbuf);

    if (!handle) {
        printf("Error opening capture file: %s\n", errbuf);
        return 1;
    }

    struct pcap_pkthdr *header;
    const u_char *packet;

    // Protocol presence flags
    int seen_eth = 0, seen_ip = 0, seen_tcp = 0, seen_udp = 0, seen_icmp = 0;

    printf("\n=============================================\n");
    printf(" TIME DIAGRAM (timestamp  |  L2 | L3 | L4)\n");
    printf("=============================================\n");

    // Read packets one-by-one
    while (pcap_next_ex(handle, &header, &packet) > 0) {

        double ts = header->ts.tv_sec + header->ts.tv_usec / 1000000.0;

        struct ether_header *eth = (struct ether_header *)packet;
        uint16_t etype = ntohs(eth->ether_type);

        char L2[] = "Ethernet";
        char L3[10] = "-";
        char L4[10] = "-";

        seen_eth = 1;

        // Check if it's IP
        if (etype == ETHERTYPE_IP) {
            seen_ip = 1;
            strcpy(L3, "IP");

            struct ip *ip_hdr = (struct ip *)(packet + sizeof(struct ether_header));

            // Check L4 protocol
            switch (ip_hdr->ip_p) {
                case IPPROTO_ICMP:
                    strcpy(L4, "ICMP");
                    seen_icmp = 1;
                    break;
                case IPPROTO_TCP:
                    strcpy(L4, "TCP");
                    seen_tcp = 1;
                    break;
                case IPPROTO_UDP:
                    strcpy(L4, "UDP");
                    seen_udp = 1;
                    break;
                default:
                    strcpy(L4, "OTHER");
            }
        }

        // Print the time diagram row
        printf("%lf  |  %s  |  %s  |  %s\n", ts, L2, L3, L4);
    }

    printf("\n=============================================\n");
    printf(" UNIQUE PROTOCOLS FOUND IN PCAP\n");
    printf("=============================================\n");

    if (seen_eth) printf("L2: Ethernet\n");
    if (seen_ip)  printf("L3: IPv4\n");
    if (seen_tcp) printf("L4: TCP\n");
    if (seen_udp) printf("L4: UDP\n");
    if (seen_icmp)printf("L4: ICMP\n");

    printf("=============================================\n");

    pcap_close(handle);
    return 0;
}

