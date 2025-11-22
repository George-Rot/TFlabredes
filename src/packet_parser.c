#include "packet_parser.h"
#include "protocols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

const char* get_protocol_name(uint8_t protocol) {
    switch (protocol) {
        case PROTO_ICMP: return "ICMP";
        case PROTO_TCP: return "TCP";
        case PROTO_UDP: return "UDP";
        case PROTO_IPV6: return "IPv6";
        default: return "OTHER";
    }
}

void format_ipv4(uint32_t ip, char *buffer) {
    sprintf(buffer, "%d.%d.%d.%d",
            (ip) & 0xFF,
            (ip >> 8) & 0xFF,
            (ip >> 16) & 0xFF,
            (ip >> 24) & 0xFF);
}

void format_ipv6(uint8_t *ip, char *buffer) {
    sprintf(buffer, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
            ip[0], ip[1], ip[2], ip[3], ip[4], ip[5], ip[6], ip[7],
            ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15]);
}

static int parse_ipv4(uint8_t *buffer, int packet_size, int offset,
                      InternetLayerInfo *inet_info,
                      TransportLayerInfo *trans_info,
                      ApplicationLayerInfo *app_info) {
    if (offset + sizeof(struct ipv4_header) > packet_size) {
        return -1;
    }

    struct ipv4_header *ip_hdr = (struct ipv4_header *)(buffer + offset);
    
    // Internet layer info
    get_timestamp(inet_info->timestamp, sizeof(inet_info->timestamp));
    strcpy(inet_info->protocol, "IPv4");
    format_ipv4(ip_hdr->src_addr, inet_info->src_ip);
    format_ipv4(ip_hdr->dest_addr, inet_info->dest_ip);
    inet_info->encapsulated_proto = ip_hdr->protocol;
    inet_info->packet_size = ntohs(ip_hdr->total_length);
    
    sprintf(inet_info->extra_info, "TTL=%d,Protocol=%s", 
            ip_hdr->ttl, get_protocol_name(ip_hdr->protocol));

    // Calculate IP header length
    int ip_hdr_len = (ip_hdr->version_ihl & 0x0F) * 4;
    int next_offset = offset + ip_hdr_len;

    // Parse transport layer
    if (ip_hdr->protocol == PROTO_TCP && next_offset + sizeof(struct tcp_header) <= packet_size) {
        struct tcp_header *tcp_hdr = (struct tcp_header *)(buffer + next_offset);
        
        get_timestamp(trans_info->timestamp, sizeof(trans_info->timestamp));
        strcpy(trans_info->protocol, "TCP");
        strcpy(trans_info->src_ip, inet_info->src_ip);
        strcpy(trans_info->dest_ip, inet_info->dest_ip);
        trans_info->src_port = ntohs(tcp_hdr->src_port);
        trans_info->dest_port = ntohs(tcp_hdr->dest_port);
        trans_info->packet_size = inet_info->packet_size;

        // Check for application layer protocols
        if (trans_info->dest_port == PORT_HTTP || trans_info->src_port == PORT_HTTP) {
            get_timestamp(app_info->timestamp, sizeof(app_info->timestamp));
            strcpy(app_info->protocol, "HTTP");
            sprintf(app_info->info, "Port=%d", trans_info->dest_port);
        } else if (trans_info->dest_port == PORT_HTTPS || trans_info->src_port == PORT_HTTPS) {
            get_timestamp(app_info->timestamp, sizeof(app_info->timestamp));
            strcpy(app_info->protocol, "HTTPS");
            sprintf(app_info->info, "Port=%d", trans_info->dest_port);
        }
        
    } else if (ip_hdr->protocol == PROTO_UDP && next_offset + sizeof(struct udp_header) <= packet_size) {
        struct udp_header *udp_hdr = (struct udp_header *)(buffer + next_offset);
        
        get_timestamp(trans_info->timestamp, sizeof(trans_info->timestamp));
        strcpy(trans_info->protocol, "UDP");
        strcpy(trans_info->src_ip, inet_info->src_ip);
        strcpy(trans_info->dest_ip, inet_info->dest_ip);
        trans_info->src_port = ntohs(udp_hdr->src_port);
        trans_info->dest_port = ntohs(udp_hdr->dest_port);
        trans_info->packet_size = ntohs(udp_hdr->length);

        // Check for application layer protocols
        if (trans_info->dest_port == PORT_DNS || trans_info->src_port == PORT_DNS) {
            get_timestamp(app_info->timestamp, sizeof(app_info->timestamp));
            strcpy(app_info->protocol, "DNS");
            sprintf(app_info->info, "Port=%d", trans_info->dest_port);
        } else if (trans_info->dest_port == PORT_DHCP_SERVER || 
                   trans_info->dest_port == PORT_DHCP_CLIENT ||
                   trans_info->src_port == PORT_DHCP_SERVER ||
                   trans_info->src_port == PORT_DHCP_CLIENT) {
            get_timestamp(app_info->timestamp, sizeof(app_info->timestamp));
            strcpy(app_info->protocol, "DHCP");
            sprintf(app_info->info, "Port=%d", trans_info->dest_port);
        } else if (trans_info->dest_port == PORT_NTP || trans_info->src_port == PORT_NTP) {
            get_timestamp(app_info->timestamp, sizeof(app_info->timestamp));
            strcpy(app_info->protocol, "NTP");
            sprintf(app_info->info, "Port=%d", trans_info->dest_port);
        }
        
    } else if (ip_hdr->protocol == PROTO_ICMP && next_offset + sizeof(struct icmp_header) <= packet_size) {
        struct icmp_header *icmp_hdr = (struct icmp_header *)(buffer + next_offset);
        
        sprintf(inet_info->extra_info, "Type=%d,Code=%d", icmp_hdr->type, icmp_hdr->code);
    }

    return 0;
}

static int parse_ipv6(uint8_t *buffer, int packet_size, int offset,
                      InternetLayerInfo *inet_info,
                      TransportLayerInfo *trans_info,
                      ApplicationLayerInfo *app_info) {
    if (offset + sizeof(struct ipv6_header) > packet_size) {
        return -1;
    }

    struct ipv6_header *ip6_hdr = (struct ipv6_header *)(buffer + offset);
    
    // Internet layer info
    get_timestamp(inet_info->timestamp, sizeof(inet_info->timestamp));
    strcpy(inet_info->protocol, "IPv6");
    format_ipv6(ip6_hdr->src_addr, inet_info->src_ip);
    format_ipv6(ip6_hdr->dest_addr, inet_info->dest_ip);
    inet_info->encapsulated_proto = ip6_hdr->next_header;
    inet_info->packet_size = ntohs(ip6_hdr->payload_length) + sizeof(struct ipv6_header);
    
    sprintf(inet_info->extra_info, "NextHeader=%s", 
            get_protocol_name(ip6_hdr->next_header));

    int next_offset = offset + sizeof(struct ipv6_header);

    // Parse transport layer (similar to IPv4)
    if (ip6_hdr->next_header == PROTO_TCP && next_offset + sizeof(struct tcp_header) <= packet_size) {
        struct tcp_header *tcp_hdr = (struct tcp_header *)(buffer + next_offset);
        
        get_timestamp(trans_info->timestamp, sizeof(trans_info->timestamp));
        strcpy(trans_info->protocol, "TCP");
        strcpy(trans_info->src_ip, inet_info->src_ip);
        strcpy(trans_info->dest_ip, inet_info->dest_ip);
        trans_info->src_port = ntohs(tcp_hdr->src_port);
        trans_info->dest_port = ntohs(tcp_hdr->dest_port);
        trans_info->packet_size = inet_info->packet_size;
        
    } else if (ip6_hdr->next_header == PROTO_UDP && next_offset + sizeof(struct udp_header) <= packet_size) {
        struct udp_header *udp_hdr = (struct udp_header *)(buffer + next_offset);
        
        get_timestamp(trans_info->timestamp, sizeof(trans_info->timestamp));
        strcpy(trans_info->protocol, "UDP");
        strcpy(trans_info->src_ip, inet_info->src_ip);
        strcpy(trans_info->dest_ip, inet_info->dest_ip);
        trans_info->src_port = ntohs(udp_hdr->src_port);
        trans_info->dest_port = ntohs(udp_hdr->dest_port);
        trans_info->packet_size = ntohs(udp_hdr->length);
    }

    return 0;
}

int parse_packet(uint8_t *buffer, int packet_size,
                 InternetLayerInfo *inet_info,
                 TransportLayerInfo *trans_info,
                 ApplicationLayerInfo *app_info) {
    
    // Clear structures
    memset(inet_info, 0, sizeof(InternetLayerInfo));
    memset(trans_info, 0, sizeof(TransportLayerInfo));
    memset(app_info, 0, sizeof(ApplicationLayerInfo));

    // For TUN interface, packets don't have Ethernet header
    // First 4 bytes are TUN header, then IP packet starts
    int offset = 4; // Skip TUN header

    if (offset >= packet_size) {
        return -1;
    }

    // Check IP version
    uint8_t version = (buffer[offset] >> 4) & 0x0F;

    if (version == 4) {
        return parse_ipv4(buffer, packet_size, offset, inet_info, trans_info, app_info);
    } else if (version == 6) {
        return parse_ipv6(buffer, packet_size, offset, inet_info, trans_info, app_info);
    }

    return -1;
}
