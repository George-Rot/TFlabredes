#include "statistics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAX_CLIENT_STATS 1000

static ProtocolCounters counters;
static ClientStats client_stats[MAX_CLIENT_STATS];
static int client_stats_count = 0;
static pthread_mutex_t stats_mutex = PTHREAD_MUTEX_INITIALIZER;

void init_statistics() {
    pthread_mutex_lock(&stats_mutex);
    memset(&counters, 0, sizeof(ProtocolCounters));
    memset(client_stats, 0, sizeof(client_stats));
    client_stats_count = 0;
    pthread_mutex_unlock(&stats_mutex);
}

void update_protocol_counters(const char *protocol, uint32_t packet_size) {
    pthread_mutex_lock(&stats_mutex);
    
    counters.total_packets++;
    counters.total_bytes += packet_size;
    
    if (strcmp(protocol, "IPv4") == 0) {
        counters.ipv4_count++;
    } else if (strcmp(protocol, "IPv6") == 0) {
        counters.ipv6_count++;
    } else if (strcmp(protocol, "ICMP") == 0) {
        counters.icmp_count++;
    } else if (strcmp(protocol, "TCP") == 0) {
        counters.tcp_count++;
    } else if (strcmp(protocol, "UDP") == 0) {
        counters.udp_count++;
    } else if (strcmp(protocol, "HTTP") == 0) {
        counters.http_count++;
    } else if (strcmp(protocol, "HTTPS") == 0) {
        counters.https_count++;
    } else if (strcmp(protocol, "DNS") == 0) {
        counters.dns_count++;
    } else if (strcmp(protocol, "DHCP") == 0) {
        counters.dhcp_count++;
    } else if (strcmp(protocol, "NTP") == 0) {
        counters.ntp_count++;
    } else {
        counters.other_count++;
    }
    
    pthread_mutex_unlock(&stats_mutex);
}

void update_client_statistics(const char *client_ip, const char *remote_ip,
                               uint16_t remote_port, const char *protocol,
                               uint32_t packet_size, int is_outbound) {
    pthread_mutex_lock(&stats_mutex);
    
    // Find or create client stats entry
    int found = -1;
    for (int i = 0; i < client_stats_count; i++) {
        if (strcmp(client_stats[i].client_ip, client_ip) == 0 &&
            strcmp(client_stats[i].remote_ip, remote_ip) == 0 &&
            client_stats[i].remote_port == remote_port &&
            strcmp(client_stats[i].protocol, protocol) == 0) {
            found = i;
            break;
        }
    }
    
    if (found == -1 && client_stats_count < MAX_CLIENT_STATS) {
        found = client_stats_count++;
        strncpy(client_stats[found].client_ip, client_ip, sizeof(client_stats[found].client_ip) - 1);
        strncpy(client_stats[found].remote_ip, remote_ip, sizeof(client_stats[found].remote_ip) - 1);
        client_stats[found].remote_port = remote_port;
        strncpy(client_stats[found].protocol, protocol, sizeof(client_stats[found].protocol) - 1);
    }
    
    if (found >= 0) {
        if (is_outbound) {
            client_stats[found].packets_sent++;
            client_stats[found].bytes_sent += packet_size;
        } else {
            client_stats[found].packets_received++;
            client_stats[found].bytes_received += packet_size;
        }
    }
    
    pthread_mutex_unlock(&stats_mutex);
}

ProtocolCounters* get_protocol_counters() {
    return &counters;
}

ClientStats* get_client_stats(int *count) {
    *count = client_stats_count;
    return client_stats;
}

void clear_statistics() {
    init_statistics();
}
