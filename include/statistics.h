#ifndef STATISTICS_H
#define STATISTICS_H

#include <stdint.h>

// Traffic statistics per client
typedef struct {
    char client_ip[64];
    char remote_ip[64];
    uint16_t remote_port;
    char protocol[16];
    uint64_t packets_sent;
    uint64_t packets_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} ClientStats;

// Protocol counters
typedef struct {
    uint64_t ipv4_count;
    uint64_t ipv6_count;
    uint64_t icmp_count;
    uint64_t tcp_count;
    uint64_t udp_count;
    uint64_t http_count;
    uint64_t https_count;
    uint64_t dns_count;
    uint64_t dhcp_count;
    uint64_t ntp_count;
    uint64_t other_count;
    uint64_t total_packets;
    uint64_t total_bytes;
} ProtocolCounters;

// Initialize statistics
void init_statistics();

// Update counters
void update_protocol_counters(const char *protocol, uint32_t packet_size);
void update_client_statistics(const char *client_ip, const char *remote_ip,
                               uint16_t remote_port, const char *protocol,
                               uint32_t packet_size, int is_outbound);

// Get statistics
ProtocolCounters* get_protocol_counters();
ClientStats* get_client_stats(int *count);

// Clear statistics
void clear_statistics();

#endif // STATISTICS_H
