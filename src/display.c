#include "display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>

static int terminal_width = 80;
static int terminal_height = 24;

void init_display() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        terminal_width = w.ws_col;
        terminal_height = w.ws_row;
    }
    
    // Clear screen
    printf("\033[2J\033[H");
    printf("=== Monitor de Tráfego de Rede em Tempo Real ===\n");
    printf("Interface: tun0\n");
    printf("Pressione Ctrl+C para sair\n\n");
    fflush(stdout);
}

void display_protocol_counters(const ProtocolCounters *counters) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           CONTADORES DE PROTOCOLOS                        ║\n");
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Total de Pacotes:    %10llu                         ║\n", counters->total_packets);
    printf("║ Total de Bytes:      %10llu                         ║\n", counters->total_bytes);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Camada Internet:                                          ║\n");
    printf("║   IPv4:              %10llu                         ║\n", counters->ipv4_count);
    printf("║   IPv6:              %10llu                         ║\n", counters->ipv6_count);
    printf("║   ICMP:              %10llu                         ║\n", counters->icmp_count);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Camada Transporte:                                        ║\n");
    printf("║   TCP:               %10llu                         ║\n", counters->tcp_count);
    printf("║   UDP:               %10llu                         ║\n", counters->udp_count);
    printf("╠════════════════════════════════════════════════════════════╣\n");
    printf("║ Camada Aplicação:                                         ║\n");
    printf("║   HTTP:              %10llu                         ║\n", counters->http_count);
    printf("║   HTTPS:             %10llu                         ║\n", counters->https_count);
    printf("║   DNS:               %10llu                         ║\n", counters->dns_count);
    printf("║   DHCP:              %10llu                         ║\n", counters->dhcp_count);
    printf("║   NTP:               %10llu                         ║\n", counters->ntp_count);
    printf("║   Outros:            %10llu                         ║\n", counters->other_count);
    printf("╚════════════════════════════════════════════════════════════╝\n");
}

void display_client_statistics() {
    int count;
    ClientStats *stats = get_client_stats(&count);
    
    if (count == 0) {
        return;
    }
    
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                        ESTATÍSTICAS POR CLIENTE                               ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║ Cliente IP       │ Remoto IP         │ Porta │ Protocolo │ Enviados │ Recebidos ║\n");
    printf("╠════════════════════════════════════════════════════════════════════════════════╣\n");
    
    for (int i = 0; i < count && i < 20; i++) {  // Show max 20 entries
        printf("║ %-15s │ %-17s │ %5d │ %-9s │ %8llu │ %9llu ║\n",
               stats[i].client_ip,
               stats[i].remote_ip,
               stats[i].remote_port,
               stats[i].protocol,
               stats[i].packets_sent,
               stats[i].packets_received);
    }
    
    if (count > 20) {
        printf("║ ... (%d entradas não mostradas)                                              ║\n", count - 20);
    }
    
    printf("╚════════════════════════════════════════════════════════════════════════════════╝\n");
}

void update_display() {
    // Move cursor to home position
    printf("\033[H");
    
    // Display header
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
    
    printf("=== Monitor de Tráfego de Rede em Tempo Real ===\n");
    printf("Interface: tun0 | Última atualização: %s\n", timestamp);
    printf("Pressione Ctrl+C para sair\n");
    
    // Display protocol counters
    ProtocolCounters *counters = get_protocol_counters();
    display_protocol_counters(counters);
    
    // Display client statistics
    display_client_statistics();
    
    fflush(stdout);
}

void cleanup_display() {
    printf("\n\nMonitoramento finalizado.\n");
}
