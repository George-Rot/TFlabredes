# Documentação Técnica - Monitor de Tráfego de Rede

## Visão Geral da Arquitetura

### Fluxo de Dados

```
Interface tun0
      ↓
[Raw Socket] ← packet_capture.c
      ↓
[Buffer de Pacotes]
      ↓
[Parser] ← packet_parser.c
      ↓
[Estruturas de Info]
      ↓
      ├→ [Logger] → CSV Files (logger.c)
      └→ [Statistics] → Contadores (statistics.c)
              ↓
         [Display] → Terminal UI (display.c)
```

## Módulos Detalhados

### 1. packet_capture.c

**Responsabilidade**: Captura de pacotes raw da interface de rede

**Funções principais**:
- `init_packet_capture()`: Cria raw socket AF_PACKET e vincula à interface
- `capture_packet()`: Recebe pacotes usando recvfrom()
- `close_packet_capture()`: Fecha o socket

**Detalhes técnicos**:
- Usa `AF_PACKET` para captura em nível de link
- Socket tipo `SOCK_RAW` com `ETH_P_ALL`
- Binding direto à interface usando `sockaddr_ll`

**Estruturas importantes**:
```c
struct sockaddr_ll {
    unsigned short sll_family;   // AF_PACKET
    unsigned short sll_protocol; // ETH_P_ALL
    int sll_ifindex;            // Interface index
    ...
};
```

### 2. packet_parser.c

**Responsabilidade**: Análise e extração de informações dos pacotes

**Funções principais**:
- `parse_packet()`: Parser principal, identifica versão IP
- `parse_ipv4()`: Processa pacotes IPv4
- `parse_ipv6()`: Processa pacotes IPv6
- `format_ipv4()` / `format_ipv6()`: Formatação de endereços

**Fluxo de parsing**:
1. Skip TUN header (4 bytes)
2. Identifica versão IP (bits 4-7 do primeiro byte)
3. Parse IP header
4. Parse Transport header (TCP/UDP)
5. Identifica Application protocol (por porta)

**Offset calculations**:
```
TUN Header:    bytes 0-3   (4 bytes)
IP Header:     bytes 4+    (20+ bytes para IPv4, 40 bytes para IPv6)
Transport:     IP_offset + IP_header_length
Application:   Transport_offset + Transport_header_length
```

### 3. logger.c

**Responsabilidade**: Escrita de logs em arquivos CSV

**Arquivos gerados**:
- `camada_internet.csv`: IPv4, IPv6, ICMP
- `camada_transporte.csv`: TCP, UDP
- `camada_aplicacao.csv`: HTTP, DNS, DHCP, NTP

**Thread safety**:
- Usa `pthread_mutex_t` para proteger escritas simultâneas
- `fflush()` após cada escrita para garantir persistência

**Formato CSV**:
```csv
# camada_internet.csv
Data/Hora,Protocolo,IP Origem,IP Destino,Protocolo Encapsulado,Info Extra,Tamanho(bytes)

# camada_transporte.csv
Data/Hora,Protocolo,IP Origem,Porta Origem,IP Destino,Porta Destino,Tamanho(bytes)

# camada_aplicacao.csv
Data/Hora,Protocolo,Informacoes
```

### 4. statistics.c

**Responsabilidade**: Manutenção de contadores e estatísticas

**Estruturas de dados**:

```c
typedef struct {
    uint64_t ipv4_count;
    uint64_t ipv6_count;
    uint64_t tcp_count;
    // ... outros contadores
    uint64_t total_packets;
    uint64_t total_bytes;
} ProtocolCounters;

typedef struct {
    char client_ip[64];      // IP do cliente túnel
    char remote_ip[64];      // IP remoto acessado
    uint16_t remote_port;    // Porta remota
    char protocol[16];       // Protocolo usado
    uint64_t packets_sent;   // Pacotes enviados
    uint64_t packets_received; // Pacotes recebidos
    uint64_t bytes_sent;     // Bytes enviados
    uint64_t bytes_received; // Bytes recebidos
} ClientStats;
```

**Thread safety**: Todas as operações protegidas por mutex

### 5. display.c

**Responsabilidade**: Interface do usuário em modo texto

**Características**:
- Detecção automática de tamanho do terminal
- Atualização periódica (a cada 50 pacotes)
- Formatação usando box-drawing characters (╔═╗║╚╝)
- Clear screen e posicionamento com ANSI escape codes

**ANSI Escape Codes usados**:
- `\033[2J`: Clear screen
- `\033[H`: Move cursor to home (0,0)

### 6. main.c

**Responsabilidade**: Loop principal e coordenação dos módulos

**Fluxo de execução**:
1. Parse argumentos de linha de comando
2. Setup signal handlers (SIGINT, SIGTERM)
3. Inicializa logger, statistics, display
4. Cria raw socket
5. Loop principal:
   - Captura pacote
   - Parse pacote
   - Log informações
   - Atualiza estatísticas
   - Atualiza display (periodicamente)
6. Cleanup ao receber sinal

## Protocolos e Estruturas

### Ethernet Header

```c
struct ethernet_header {
    uint8_t dest_mac[6];     // MAC destino
    uint8_t src_mac[6];      // MAC origem
    uint16_t ether_type;     // 0x0800 (IPv4), 0x86DD (IPv6)
} __attribute__((packed));
```

### IPv4 Header

```c
struct ipv4_header {
    uint8_t version_ihl;     // Version (4 bits) + IHL (4 bits)
    uint8_t tos;             // Type of Service
    uint16_t total_length;   // Total length
    uint16_t identification; // Identification
    uint16_t flags_fragment; // Flags + Fragment offset
    uint8_t ttl;            // Time to Live
    uint8_t protocol;       // Protocol (1=ICMP, 6=TCP, 17=UDP)
    uint16_t checksum;      // Header checksum
    uint32_t src_addr;      // Source address
    uint32_t dest_addr;     // Destination address
} __attribute__((packed));
```

**Cálculo do IHL (Internet Header Length)**:
```c
int header_length = (version_ihl & 0x0F) * 4;  // Em bytes
```

### TCP Header

```c
struct tcp_header {
    uint16_t src_port;       // Porta origem
    uint16_t dest_port;      // Porta destino
    uint32_t seq_number;     // Sequence number
    uint32_t ack_number;     // Acknowledgment number
    uint8_t data_offset_flags; // Data offset + Reserved
    uint8_t flags;           // Flags (SYN, ACK, FIN, etc.)
    uint16_t window;         // Window size
    uint16_t checksum;       // Checksum
    uint16_t urgent_pointer; // Urgent pointer
} __attribute__((packed));
```

**TCP Flags**:
- FIN: 0x01
- SYN: 0x02
- RST: 0x04
- PSH: 0x08
- ACK: 0x10
- URG: 0x20

### UDP Header

```c
struct udp_header {
    uint16_t src_port;    // Porta origem
    uint16_t dest_port;   // Porta destino
    uint16_t length;      // Comprimento (header + data)
    uint16_t checksum;    // Checksum
} __attribute__((packed));
```

## Identificação de Protocolos de Aplicação

A identificação é baseada em portas conhecidas (well-known ports):

| Protocolo | Porta(s) | Descrição |
|-----------|----------|-----------|
| HTTP      | 80       | HyperText Transfer Protocol |
| HTTPS     | 443      | HTTP Secure |
| DNS       | 53       | Domain Name System |
| DHCP      | 67/68    | Dynamic Host Configuration Protocol |
| NTP       | 123      | Network Time Protocol |

**Código de detecção**:
```c
if (dest_port == 80 || src_port == 80) {
    strcpy(protocol, "HTTP");
} else if (dest_port == 443 || src_port == 443) {
    strcpy(protocol, "HTTPS");
}
// ... etc
```

## Interface TUN/TAP

### Estrutura do Pacote na Interface TUN

```
┌─────────────┬─────────────────────────────┐
│ TUN Header  │      IP Packet              │
│  (4 bytes)  │  (IPv4 ou IPv6)             │
└─────────────┴─────────────────────────────┘
```

**TUN Header**:
- 2 bytes: flags
- 2 bytes: protocol (0x0800 = IPv4, 0x86DD = IPv6)

**Diferença entre TUN e TAP**:
- **TUN**: Trabalha no nível 3 (IP) - sem Ethernet header
- **TAP**: Trabalha no nível 2 (Link) - com Ethernet header

### Por que usar TUN?

O programa túnel fornecido cria uma interface TUN porque:
1. Não precisa de MAC addresses (trabalha com IPs)
2. Mais eficiente para VPN/túneis IP
3. Mais simples de implementar roteamento

## Considerações de Performance

### Buffer Size

```c
#define BUFFER_SIZE 65536  // 64KB
```

**Justificativa**:
- MTU padrão Ethernet: 1500 bytes
- Jumbo frames: até 9000 bytes
- Buffer de 64KB garante captura de qualquer pacote IP válido

### Atualização de Display

```c
if (display_counter % 50 == 0) {
    update_display();
}
```

**Justificativa**:
- Atualizar display a cada pacote seria muito lento
- 50 pacotes é um bom balanço entre responsividade e performance

### Thread Safety

**Mutexes utilizados**:
- `log_mutex`: Protege escritas nos arquivos de log
- `stats_mutex`: Protege atualização de contadores

**Padrão de uso**:
```c
pthread_mutex_lock(&mutex);
// ... operação crítica ...
pthread_mutex_unlock(&mutex);
```

## Endianness e Network Byte Order

**Network byte order**: Big-endian

**Conversões necessárias**:
```c
uint16_t port = ntohs(tcp_header->dest_port);  // Network to Host Short
uint32_t addr = ntohl(ip_header->src_addr);    // Network to Host Long
```

**Funções de conversão**:
- `ntohs()`: Network to Host Short (16 bits)
- `ntohl()`: Network to Host Long (32 bits)
- `htons()`: Host to Network Short
- `htonl()`: Host to Network Long

## Debugging e Troubleshooting

### Habilitar logs de debug

Adicione no código:
```c
#define DEBUG 1

#ifdef DEBUG
    printf("DEBUG: Pacote capturado, tamanho=%d\n", packet_size);
#endif
```

### Verificar captura de pacotes

```bash
# Verificar se pacotes estão chegando na interface
sudo tcpdump -i tun0 -n

# Contar pacotes
sudo tcpdump -i tun0 -n | wc -l
```

### Verificar privilégios

```bash
# Verificar capabilities
getcap ./network_monitor

# Adicionar capability (alternativa ao sudo)
sudo setcap cap_net_raw+ep ./network_monitor
```

### Analisar logs

```bash
# Contar pacotes por protocolo
cut -d',' -f2 logs/camada_internet.csv | sort | uniq -c

# Top 10 IPs mais ativos
cut -d',' -f3 logs/camada_internet.csv | sort | uniq -c | sort -rn | head -10
```

## Extensões Futuras

### 1. Suporte a PCAP

```c
#include <pcap.h>

pcap_t *handle = pcap_open_offline("capture.pcap", errbuf);
pcap_loop(handle, -1, packet_handler, NULL);
```

### 2. Deep Packet Inspection

```c
// Analisar payload HTTP
if (protocol == HTTP) {
    char *payload = packet + transport_offset + tcp_header_length;
    if (strncmp(payload, "GET ", 4) == 0) {
        // Parse HTTP GET request
    }
}
```

### 3. Filtragem BPF

```c
struct sock_fprog bpf = {
    .len = filter_len,
    .filter = filter_code
};
setsockopt(sockfd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf));
```

## Referências Técnicas

### RFCs Importantes

- RFC 791: Internet Protocol (IPv4)
- RFC 793: Transmission Control Protocol
- RFC 768: User Datagram Protocol
- RFC 792: Internet Control Message Protocol
- RFC 2460: Internet Protocol Version 6 (IPv6)
- RFC 2616: Hypertext Transfer Protocol (HTTP/1.1)

### Man Pages Relevantes

```bash
man 7 packet    # AF_PACKET
man 7 raw       # Raw sockets
man 2 socket    # Socket API
man 2 bind      # Bind socket to interface
man 7 netdevice # Network device ioctls
```

### Ferramentas de Análise

- **Wireshark**: Análise gráfica de pacotes
- **tcpdump**: Captura de pacotes CLI
- **tshark**: Wireshark CLI
- **netstat**: Estatísticas de rede
- **ss**: Socket statistics
- **iftop**: Monitoramento de banda por conexão
