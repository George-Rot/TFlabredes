#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <stdint.h>
#include <time.h>

// Packet information structures
typedef struct {
    char timestamp[32];
    char protocol[16];
    char src_ip[64];
    char dest_ip[64];
    uint8_t encapsulated_proto;
    char extra_info[256];
    uint32_t packet_size;
} InternetLayerInfo;

typedef struct {
    char timestamp[32];
    char protocol[16];
    char src_ip[64];
    uint16_t src_port;
    char dest_ip[64];
    uint16_t dest_port;
    uint32_t packet_size;
} TransportLayerInfo;

typedef struct {
    char timestamp[32];
    char protocol[16];
    char info[512];
} ApplicationLayerInfo;

// Parse packet and extract information
int parse_packet(uint8_t *buffer, int packet_size, 
                 InternetLayerInfo *inet_info,
                 TransportLayerInfo *trans_info,
                 ApplicationLayerInfo *app_info);

// Helper functions
void get_timestamp(char *buffer, size_t size);
const char* get_protocol_name(uint8_t protocol);
void format_ipv4(uint32_t ip, char *buffer);
void format_ipv6(uint8_t *ip, char *buffer);

#endif // PACKET_PARSER_H
