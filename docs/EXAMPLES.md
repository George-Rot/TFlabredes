# Exemplos de Saída do Monitor

## Exemplo 1: Interface do Monitor em Execução

```
=== Monitor de Tráfego de Rede em Tempo Real ===
Interface: tun0 | Última atualização: 2024-11-22 15:30:45
Pressione Ctrl+C para sair


╔════════════════════════════════════════════════════════════╗
║           CONTADORES DE PROTOCOLOS                        ║
╠════════════════════════════════════════════════════════════╣
║ Total de Pacotes:         1523                           ║
║ Total de Bytes:        1245678                           ║
╠════════════════════════════════════════════════════════════╣
║ Camada Internet:                                          ║
║   IPv4:                   1450                           ║
║   IPv6:                     48                           ║
║   ICMP:                     25                           ║
╠════════════════════════════════════════════════════════════╣
║ Camada Transporte:                                        ║
║   TCP:                     987                           ║
║   UDP:                     463                           ║
╠════════════════════════════════════════════════════════════╣
║ Camada Aplicação:                                         ║
║   HTTP:                    123                           ║
║   HTTPS:                   654                           ║
║   DNS:                     289                           ║
║   DHCP:                      8                           ║
║   NTP:                      12                           ║
║   Outros:                   15                           ║
╚════════════════════════════════════════════════════════════╝

╔════════════════════════════════════════════════════════════════════════════════╗
║                        ESTATÍSTICAS POR CLIENTE                               ║
╠════════════════════════════════════════════════════════════════════════════════╣
║ Cliente IP       │ Remoto IP         │ Porta │ Protocolo │ Enviados │ Recebidos ║
╠════════════════════════════════════════════════════════════════════════════════╣
║ 172.31.66.2      │ 8.8.8.8           │    53 │ UDP       │       45 │        45 ║
║ 172.31.66.2      │ 142.250.185.78    │   443 │ TCP       │      234 │       189 ║
║ 172.31.66.2      │ 93.184.216.34     │    80 │ TCP       │       23 │        21 ║
║ 172.31.66.3      │ 1.1.1.1           │    53 │ UDP       │       12 │        12 ║
║ 172.31.66.3      │ 140.82.121.4      │   443 │ TCP       │       78 │        65 ║
╚════════════════════════════════════════════════════════════════════════════════╝
```

## Exemplo 2: camada_internet.csv

```csv
Data/Hora,Protocolo,IP Origem,IP Destino,Protocolo Encapsulado,Info Extra,Tamanho(bytes)
2024-11-22 15:30:01,IPv4,172.31.66.2,8.8.8.8,17,TTL=64,Protocol=UDP,76
2024-11-22 15:30:01,IPv4,8.8.8.8,172.31.66.2,17,TTL=115,Protocol=UDP,92
2024-11-22 15:30:02,IPv4,172.31.66.2,142.250.185.78,6,TTL=64,Protocol=TCP,60
2024-11-22 15:30:02,IPv4,142.250.185.78,172.31.66.2,6,TTL=54,Protocol=TCP,60
2024-11-22 15:30:02,IPv4,172.31.66.2,142.250.185.78,6,TTL=64,Protocol=TCP,52
2024-11-22 15:30:03,IPv4,172.31.66.2,142.250.185.78,6,TTL=64,Protocol=TCP,1500
2024-11-22 15:30:03,IPv4,142.250.185.78,172.31.66.2,6,TTL=54,Protocol=TCP,52
2024-11-22 15:30:05,IPv4,172.31.66.3,1.1.1.1,17,TTL=64,Protocol=UDP,65
2024-11-22 15:30:05,ICMP,172.31.66.2,8.8.8.8,1,Type=8,Code=0,84
2024-11-22 15:30:05,ICMP,8.8.8.8,172.31.66.2,1,Type=0,Code=0,84
2024-11-22 15:30:10,IPv6,fe80::1234:5678:abcd:ef01,2001:4860:4860::8888,17,NextHeader=UDP,1280
```

## Exemplo 3: camada_transporte.csv

```csv
Data/Hora,Protocolo,IP Origem,Porta Origem,IP Destino,Porta Destino,Tamanho(bytes)
2024-11-22 15:30:01,UDP,172.31.66.2,54321,8.8.8.8,53,76
2024-11-22 15:30:01,UDP,8.8.8.8,53,172.31.66.2,54321,92
2024-11-22 15:30:02,TCP,172.31.66.2,45678,142.250.185.78,443,60
2024-11-22 15:30:02,TCP,142.250.185.78,443,172.31.66.2,45678,60
2024-11-22 15:30:02,TCP,172.31.66.2,45678,142.250.185.78,443,52
2024-11-22 15:30:03,TCP,172.31.66.2,45678,142.250.185.78,443,1500
2024-11-22 15:30:03,TCP,142.250.185.78,443,172.31.66.2,45678,52
2024-11-22 15:30:05,UDP,172.31.66.3,49876,1.1.1.1,53,65
2024-11-22 15:30:07,TCP,172.31.66.2,38291,93.184.216.34,80,345
2024-11-22 15:30:08,TCP,93.184.216.34,80,172.31.66.2,38291,1460
2024-11-22 15:30:12,UDP,172.31.66.2,123,129.6.15.28,123,48
```

## Exemplo 4: camada_aplicacao.csv

```csv
Data/Hora,Protocolo,Informacoes
2024-11-22 15:30:01,DNS,Port=53
2024-11-22 15:30:01,DNS,Port=53
2024-11-22 15:30:02,HTTPS,Port=443
2024-11-22 15:30:02,HTTPS,Port=443
2024-11-22 15:30:05,DNS,Port=53
2024-11-22 15:30:07,HTTP,Port=80
2024-11-22 15:30:08,HTTP,Port=80
2024-11-22 15:30:12,NTP,Port=123
2024-11-22 15:30:15,DHCP,Port=67
2024-11-22 15:30:15,DHCP,Port=68
```

## Exemplo 5: Saída do Script analyze_logs.sh

```
╔════════════════════════════════════════════════════════════╗
║        ANÁLISE DE LOGS - Monitor de Tráfego                ║
╚════════════════════════════════════════════════════════════╝

1. ESTATÍSTICAS GERAIS
─────────────────────────────────────────────────────────────
Total de pacotes capturados:
  Camada Internet:   1523
  Camada Transporte: 1450
  Camada Aplicação:  897

2. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA INTERNET
─────────────────────────────────────────────────────────────
   1450 IPv4
     48 IPv6
     25 ICMP

3. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA TRANSPORTE
─────────────────────────────────────────────────────────────
    987 TCP
    463 UDP

4. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA APLICAÇÃO
─────────────────────────────────────────────────────────────
    654 HTTPS
    289 DNS
    123 HTTP
     12 NTP
      8 DHCP

5. TOP 10 IPs ORIGEM
─────────────────────────────────────────────────────────────
    456 172.31.66.2
    234 142.250.185.78
    198 172.31.66.3
    167 8.8.8.8
     89 93.184.216.34
     67 1.1.1.1
     45 140.82.121.4
     23 172.31.66.5

6. TOP 10 IPs DESTINO
─────────────────────────────────────────────────────────────
    423 172.31.66.2
    267 142.250.185.78
    178 8.8.8.8
    156 172.31.66.3
     98 93.184.216.34
     76 1.1.1.1
     54 140.82.121.4
     32 172.31.66.5

7. TOP 10 PORTAS DESTINO
─────────────────────────────────────────────────────────────
    654 443
    289 53
    123 80
     45 49876
     34 54321
     23 45678
     12 123
      8 67

8. CLIENTES DO TÚNEL ATIVOS (172.31.66.x)
─────────────────────────────────────────────────────────────
172.31.66.2
172.31.66.3
172.31.66.5

9. TRÁFEGO POR CLIENTE (Pacotes)
─────────────────────────────────────────────────────────────
Cliente IP         | Origem | Destino
───────────────────┼────────┼─────────
172.31.66.2        |    456 |     423
172.31.66.3        |    198 |     156
172.31.66.5        |     23 |      32

10. SERVIÇOS MAIS ACESSADOS
─────────────────────────────────────────────────────────────
Protocolo | Acessos
──────────┼─────────
HTTPS     |     654
DNS       |     289
HTTP      |     123
NTP       |      12
DHCP      |       8

11. ANÁLISE TEMPORAL (Últimos 10 pacotes)
─────────────────────────────────────────────────────────────
Timestamp           | Protocolo | Origem → Destino
────────────────────┼───────────┼─────────────────────────
2024-11-22 15:30:40 | IPv4      | 172.31.66.2 → 142.250.185.78
2024-11-22 15:30:41 | IPv4      | 142.250.185.78 → 172.31.66.2
2024-11-22 15:30:42 | IPv4      | 172.31.66.3 → 8.8.8.8
2024-11-22 15:30:42 | IPv4      | 8.8.8.8 → 172.31.66.3
2024-11-22 15:30:43 | IPv4      | 172.31.66.2 → 93.184.216.34
2024-11-22 15:30:43 | IPv4      | 93.184.216.34 → 172.31.66.2
2024-11-22 15:30:44 | ICMP      | 172.31.66.2 → 8.8.8.8
2024-11-22 15:30:44 | ICMP      | 8.8.8.8 → 172.31.66.2
2024-11-22 15:30:45 | IPv4      | 172.31.66.3 → 1.1.1.1
2024-11-22 15:30:45 | IPv4      | 1.1.1.1 → 172.31.66.3

12. ESTATÍSTICAS DE TAMANHO DE PACOTES
─────────────────────────────────────────────────────────────
  Mínimo:  52 bytes
  Máximo:  1500 bytes
  Média:   817.32 bytes
  Total:   1216.18 KB

13. CONEXÕES POR CLIENTE (Camada Transporte)
─────────────────────────────────────────────────────────────
Cliente          | Destino          | Porta | Protocolo | Count
─────────────────┼──────────────────┼───────┼───────────┼──────
172.31.66.2      | 142.250.185.78   | 443   | TCP       | 234
172.31.66.2      | 8.8.8.8          | 53    | UDP       | 145
172.31.66.2      | 93.184.216.34    | 80    | TCP       | 89
172.31.66.3      | 8.8.8.8          | 53    | UDP       | 67
172.31.66.3      | 1.1.1.1          | 53    | UDP       | 45
172.31.66.3      | 140.82.121.4     | 443   | TCP       | 34

╔════════════════════════════════════════════════════════════╗
║                   FIM DA ANÁLISE                           ║
╚════════════════════════════════════════════════════════════╝
```

## Exemplo 6: Comandos Úteis para Análise Manual

### Buscar tráfego específico de um cliente

```bash
# Todos os pacotes do cliente 172.31.66.2
grep "172.31.66.2" logs/camada_internet.csv

# Apenas pacotes de saída (origem)
grep "^[^,]*,[^,]*,172.31.66.2," logs/camada_internet.csv

# Apenas pacotes de entrada (destino)
grep ",[^,]*,172.31.66.2$" logs/camada_internet.csv
```

### Filtrar por protocolo

```bash
# Apenas pacotes TCP
grep ",TCP," logs/camada_transporte.csv

# Apenas pacotes HTTPS
grep ",HTTPS," logs/camada_aplicacao.csv

# Contar pacotes ICMP
grep ",ICMP," logs/camada_internet.csv | wc -l
```

### Análise por período

```bash
# Pacotes entre 15:30 e 15:40
grep "2024-11-22 15:3[0-9]:" logs/camada_internet.csv

# Últimos 100 pacotes
tail -100 logs/camada_internet.csv
```

### Análise de portas

```bash
# Conexões na porta 443 (HTTPS)
grep ",443," logs/camada_transporte.csv

# Todas as portas usadas por um cliente
grep "^[^,]*,[^,]*,172.31.66.2," logs/camada_transporte.csv | cut -d',' -f6 | sort -u
```

### Análise de bandwidth

```bash
# Soma total de bytes (camada internet)
tail -n +2 logs/camada_internet.csv | cut -d',' -f7 | paste -sd+ | bc

# Bytes por protocolo
for proto in IPv4 IPv6 TCP UDP; do
    echo -n "$proto: "
    grep ",$proto," logs/camada_internet.csv logs/camada_transporte.csv 2>/dev/null | \
        cut -d',' -f7 | paste -sd+ | bc
    echo " bytes"
done
```

## Exemplo 7: Monitoramento em Tempo Real

### Terminal 1: Executar o monitor
```bash
sudo ./network_monitor -i tun0 -l ./logs
```

### Terminal 2: Watch dos logs
```bash
# Monitorar adições ao log de internet
watch -n 1 "tail -10 logs/camada_internet.csv"

# Ou usar tail -f
tail -f logs/camada_internet.csv
```

### Terminal 3: Estatísticas ao vivo
```bash
# Atualizar análise a cada 5 segundos
watch -n 5 "./analyze_logs.sh"

# Ou contadores simples
watch -n 1 'echo "IPv4: $(grep ",IPv4," logs/camada_internet.csv | wc -l)"; \
             echo "TCP:  $(grep ",TCP," logs/camada_transporte.csv | wc -l)"; \
             echo "DNS:  $(grep ",DNS," logs/camada_aplicacao.csv | wc -l)"'
```

## Exemplo 8: Cenários de Teste

### Cenário 1: Navegação Web (Cliente)

```bash
# Gerar tráfego HTTP/HTTPS
curl http://example.com
curl https://www.google.com
wget http://httpbin.org/get
```

**Resultado esperado nos logs**:
- DNS queries (porta 53, UDP)
- TCP handshake (SYN, SYN-ACK, ACK)
- HTTP/HTTPS (portas 80/443)

### Cenário 2: Teste de Conectividade (Cliente)

```bash
# ICMP echo request/reply
ping -c 5 8.8.8.8
ping -c 5 www.google.com

# Traceroute (ICMP/UDP)
traceroute 8.8.8.8
```

**Resultado esperado**:
- ICMP packets (Type 8 e Type 0)
- DNS query para resolver nomes
- Multiple hops se usar traceroute

### Cenário 3: Download de Arquivo (Cliente)

```bash
# Download grande para ver fragmentação
wget https://speed.hetzner.de/100MB.bin
```

**Resultado esperado**:
- Muitos pacotes TCP
- Pacotes de 1500 bytes (MTU)
- Alto volume de bytes em logs

### Cenário 4: DNS Intensivo (Cliente)

```bash
# Múltiplas queries DNS
for domain in google.com facebook.com twitter.com github.com; do
    nslookup $domain
    nslookup $domain 1.1.1.1
done
```

**Resultado esperado**:
- Pacotes UDP porta 53
- Queries para diferentes servidores DNS
- Entradas em camada_aplicacao.csv com DNS
