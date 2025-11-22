#include "packet_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <errno.h>

int init_packet_capture(const char *interface) {
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_ll sll;

    // Create raw socket
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0) {
        perror("Error creating raw socket");
        return -1;
    }

    // Get interface index
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Error getting interface index");
        close(sockfd);
        return -1;
    }

    // Bind socket to interface
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    if (bind(sockfd, (struct sockaddr *)&sll, sizeof(sll)) < 0) {
        perror("Error binding socket to interface");
        close(sockfd);
        return -1;
    }

    printf("Raw socket initialized on interface %s (index: %d)\n", 
           interface, ifr.ifr_ifindex);

    return sockfd;
}

int capture_packet(int sockfd, uint8_t *buffer, int buffer_size) {
    int packet_size;
    
    packet_size = recvfrom(sockfd, buffer, buffer_size, 0, NULL, NULL);
    
    if (packet_size < 0) {
        if (errno != EINTR) {
            perror("Error receiving packet");
        }
        return -1;
    }
    
    return packet_size;
}

void close_packet_capture(int sockfd) {
    if (sockfd >= 0) {
        close(sockfd);
        printf("Packet capture socket closed\n");
    }
}
