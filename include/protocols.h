#ifndef PROTOCOLS_H
#define PROTOCOLS_H

#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Ethernet Header
struct ethernet_header {
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
    uint16_t ether_type;
} __attribute__((packed));

// IPv4 Header
struct ipv4_header {
    uint8_t version_ihl;
    uint8_t tos;
    uint16_t total_length;
    uint16_t identification;
    uint16_t flags_fragment;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t src_addr;
    uint32_t dest_addr;
} __attribute__((packed));

// IPv6 Header
struct ipv6_header {
    uint32_t version_class_label;
    uint16_t payload_length;
    uint8_t next_header;
    uint8_t hop_limit;
    uint8_t src_addr[16];
    uint8_t dest_addr[16];
} __attribute__((packed));

// TCP Header
struct tcp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint32_t seq_number;
    uint32_t ack_number;
    uint8_t data_offset_flags;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgent_pointer;
} __attribute__((packed));

// UDP Header
struct udp_header {
    uint16_t src_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed));

// ICMP Header
struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence;
} __attribute__((packed));

// Protocol numbers
#define PROTO_ICMP 1
#define PROTO_TCP 6
#define PROTO_UDP 17
#define PROTO_IPV6 41

// Ether types
#define ETHER_TYPE_IPV4 0x0800
#define ETHER_TYPE_IPV6 0x86DD

// Common ports
#define PORT_HTTP 80
#define PORT_HTTPS 443
#define PORT_DNS 53
#define PORT_DHCP_SERVER 67
#define PORT_DHCP_CLIENT 68
#define PORT_NTP 123

#endif // PROTOCOLS_H
