#ifndef PACKET_CAPTURE_H
#define PACKET_CAPTURE_H

#include <stdint.h>

// Initialize raw socket for packet capture
int init_packet_capture(const char *interface);

// Capture a single packet
int capture_packet(int sockfd, uint8_t *buffer, int buffer_size);

// Close socket
void close_packet_capture(int sockfd);

#endif // PACKET_CAPTURE_H
