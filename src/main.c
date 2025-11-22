#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "packet_capture.h"
#include "packet_parser.h"
#include "logger.h"
#include "statistics.h"
#include "display.h"

#define BUFFER_SIZE 65536
#define DEFAULT_INTERFACE "tun0"
#define LOG_DIRECTORY "./logs"
#define DISPLAY_UPDATE_INTERVAL 2  // seconds

static volatile int running = 1;
static int display_counter = 0;

void signal_handler(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        running = 0;
        printf("\n\nRecebido sinal de interrupção. Finalizando...\n");
    }
}

void print_usage(const char *program_name) {
    printf("Uso: %s [-i interface] [-l log_dir] [-h]\n", program_name);
    printf("Opções:\n");
    printf("  -i interface  Interface de rede a ser monitorada (padrão: tun0)\n");
    printf("  -l log_dir    Diretório para arquivos de log (padrão: ./logs)\n");
    printf("  -h            Exibe esta mensagem de ajuda\n");
}

int is_tunnel_client_ip(const char *ip) {
    // Check if IP belongs to tunnel network (172.31.66.x)
    return (strncmp(ip, "172.31.66.", 10) == 0);
}

int main(int argc, char *argv[]) {
    int opt;
    const char *interface = DEFAULT_INTERFACE;
    const char *log_dir = LOG_DIRECTORY;
    
    // Parse command line arguments
    while ((opt = getopt(argc, argv, "i:l:h")) != -1) {
        switch (opt) {
            case 'i':
                interface = optarg;
                break;
            case 'l':
                log_dir = optarg;
                break;
            case 'h':
                print_usage(argv[0]);
                return 0;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize components
    printf("Inicializando Monitor de Tráfego de Rede...\n");
    
    if (init_logger(log_dir) < 0) {
        fprintf(stderr, "Erro ao inicializar sistema de logs\n");
        return 1;
    }
    
    init_statistics();
    init_display();
    
    // Initialize packet capture
    int sockfd = init_packet_capture(interface);
    if (sockfd < 0) {
        fprintf(stderr, "Erro ao inicializar captura de pacotes\n");
        fprintf(stderr, "Certifique-se de executar com privilégios de root/sudo\n");
        close_logger();
        return 1;
    }
    
    printf("Monitoramento iniciado na interface %s\n", interface);
    printf("Logs sendo salvos em %s/\n", log_dir);
    printf("\n");
    
    // Packet buffer
    uint8_t buffer[BUFFER_SIZE];
    
    // Main capture loop
    while (running) {
        int packet_size = capture_packet(sockfd, buffer, BUFFER_SIZE);
        
        if (packet_size < 0) {
            if (running) {
                continue;  // Error or interrupt, try again
            }
            break;
        }
        
        // Parse packet
        InternetLayerInfo inet_info;
        TransportLayerInfo trans_info;
        ApplicationLayerInfo app_info;
        
        if (parse_packet(buffer, packet_size, &inet_info, &trans_info, &app_info) == 0) {
            // Log packet information
            if (inet_info.protocol[0]) {
                log_internet_layer(&inet_info);
                update_protocol_counters(inet_info.protocol, inet_info.packet_size);
            }
            
            if (trans_info.protocol[0]) {
                log_transport_layer(&trans_info);
                update_protocol_counters(trans_info.protocol, trans_info.packet_size);
                
                // Update client statistics
                if (is_tunnel_client_ip(trans_info.src_ip)) {
                    update_client_statistics(trans_info.src_ip, trans_info.dest_ip,
                                           trans_info.dest_port, trans_info.protocol,
                                           trans_info.packet_size, 1);
                } else if (is_tunnel_client_ip(trans_info.dest_ip)) {
                    update_client_statistics(trans_info.dest_ip, trans_info.src_ip,
                                           trans_info.src_port, trans_info.protocol,
                                           trans_info.packet_size, 0);
                }
            }
            
            if (app_info.protocol[0]) {
                log_application_layer(&app_info);
                update_protocol_counters(app_info.protocol, 0);
            }
            
            // Update display periodically
            display_counter++;
            if (display_counter % 50 == 0) {  // Update every 50 packets
                update_display();
            }
        }
    }
    
    // Cleanup
    printf("\n\nFinalizando monitor...\n");
    close_packet_capture(sockfd);
    close_logger();
    cleanup_display();
    
    printf("Monitor finalizado com sucesso.\n");
    printf("Logs salvos em %s/\n", log_dir);
    
    return 0;
}
