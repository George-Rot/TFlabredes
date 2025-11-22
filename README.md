# Monitor de Tráfego de Rede em Tempo Real

## Descrição

Monitor de tráfego de rede desenvolvido em C que utiliza raw sockets para capturar e analisar pacotes em tempo real na interface `tun0`. O sistema identifica e classifica diferentes protocolos (IPv4, IPv6, TCP, UDP, ICMP, HTTP, HTTPS, DNS, DHCP, NTP) e gera logs detalhados em formato CSV.

## Características

- ✅ Captura de pacotes usando raw sockets
- ✅ Análise de protocolos das camadas Internet, Transporte e Aplicação
- ✅ Geração de logs CSV em tempo real
- ✅ Interface modo texto com estatísticas ao vivo
- ✅ Contadores de protocolo por tipo
- ✅ Estatísticas por cliente (IP do túnel)
- ✅ Thread-safe para operações concorrentes

## Arquitetura

O sistema é composto pelos seguintes módulos:

```
network_monitor/
├── include/              # Arquivos de cabeçalho
│   ├── protocols.h       # Estruturas dos protocolos
│   ├── packet_capture.h  # Captura de pacotes
│   ├── packet_parser.h   # Parser de pacotes
│   ├── logger.h          # Sistema de logging
│   ├── statistics.h      # Estatísticas e contadores
│   └── display.h         # Interface do usuário
├── src/                  # Código fonte
│   ├── main.c
│   ├── packet_capture.c
│   ├── packet_parser.c
│   ├── logger.c
│   ├── statistics.c
│   └── display.c
├── logs/                 # Arquivos de log CSV
│   ├── camada_internet.csv
│   ├── camada_transporte.csv
│   └── camada_aplicacao.csv
├── Makefile
└── README.md
```

## Requisitos

### Sistema Operacional
- Linux (testado em Ubuntu/Debian)
- Kernel com suporte a raw sockets e interface TUN/TAP

### Dependências
- GCC (GNU Compiler Collection)
- build-essential
- pthread (normalmente incluído)

### Instalação de Dependências

```bash
sudo apt-get update
sudo apt-get install build-essential
```

Ou use o Makefile:

```bash
make install-deps
```

## Compilação

### Compilação Simples

```bash
make
```

### Limpeza

```bash
make clean          # Remove arquivos de compilação
make clean-logs     # Remove logs
make cleanall       # Remove tudo
```

## Uso

### Execução Básica

O programa **requer privilégios de root** para criar raw sockets:

```bash
sudo ./network_monitor
```

### Opções de Linha de Comando

```bash
sudo ./network_monitor [-i interface] [-l log_dir] [-h]

Opções:
  -i interface  Interface de rede a ser monitorada (padrão: tun0)
  -l log_dir    Diretório para arquivos de log (padrão: ./logs)
  -h            Exibe mensagem de ajuda
```

### Exemplos

```bash
# Monitorar interface tun0 (padrão)
sudo ./network_monitor

# Monitorar interface específica
sudo ./network_monitor -i eth0

# Especificar diretório de logs
sudo ./network_monitor -l /var/log/network_monitor

# Executar via Makefile
make run
```

## Arquivos de Log

### camada_internet.csv

Contém informações da camada Internet (IPv4, IPv6, ICMP):

```csv
Data/Hora,Protocolo,IP Origem,IP Destino,Protocolo Encapsulado,Info Extra,Tamanho(bytes)
2024-01-15 10:30:45,IPv4,172.31.66.2,8.8.8.8,6,TTL=64,Protocol=TCP,60
```

### camada_transporte.csv

Contém informações da camada de Transporte (TCP, UDP):

```csv
Data/Hora,Protocolo,IP Origem,Porta Origem,IP Destino,Porta Destino,Tamanho(bytes)
2024-01-15 10:30:45,TCP,172.31.66.2,54321,8.8.8.8,443,60
```

### camada_aplicacao.csv

Contém informações da camada de Aplicação (HTTP, HTTPS, DNS, DHCP, NTP):

```csv
Data/Hora,Protocolo,Informacoes
2024-01-15 10:30:45,HTTPS,Port=443
```

## Visualização dos Logs

Durante a execução, os logs são atualizados em tempo real. Você pode visualizá-los com:

```bash
# Em outro terminal
cat logs/camada_internet.csv
cat logs/camada_transporte.csv
cat logs/camada_aplicacao.csv

# Monitoramento contínuo
tail -f logs/camada_internet.csv
```

## Interface do Usuário

A interface modo texto exibe:

1. **Contadores de Protocolos**:
   - Total de pacotes e bytes
   - Contadores por protocolo (IPv4, IPv6, TCP, UDP, etc.)

2. **Estatísticas por Cliente**:
   - IP do cliente (túnel)
   - IP remoto acessado
   - Porta e protocolo
   - Pacotes e bytes enviados/recebidos

3. **Atualização em Tempo Real**:
   - A interface é atualizada automaticamente a cada 50 pacotes capturados

## Funcionamento com o Túnel

### Arquitetura do Túnel

O monitor deve ser executado no **servidor proxy**, onde a interface `tun0` está configurada:

```
Cliente (172.31.66.x) → tun0 (programa túnel) → eth0 → Internet
                          ↑
                     [MONITOR AQUI]
```

### Rede do Túnel

- Interface: `tun0`
- Rede: `172.31.66.0/24`
- Servidor: `172.31.66.1`
- Clientes: `172.31.66.2` - `172.31.66.254`

### Preparação do Ambiente

1. **Compile o traffic_tunnel** (fornecido pelo professor):
   ```bash
   make  # no diretório do túnel
   ```

2. **Execute o túnel no servidor**:
   ```bash
   sudo ./traffic_tunnel <interface> -s
   ```

3. **Execute o túnel nos clientes**:
   ```bash
   sudo ./traffic_tunnel <interface> -c client1.sh
   ```

4. **Execute o monitor no servidor**:
   ```bash
   sudo ./network_monitor -i tun0
   ```

## Protocolos Suportados

### Camada Internet
- IPv4
- IPv6
- ICMP (ping, traceroute)

### Camada Transporte
- TCP (conexões, flags)
- UDP (datagramas)

### Camada Aplicação
- HTTP (porta 80)
- HTTPS (porta 443)
- DNS (porta 53)
- DHCP (portas 67/68)
- NTP (porta 123)

## Estruturas de Dados

### Cabeçalhos de Protocolos

O arquivo `protocols.h` define estruturas para:
- Ethernet Header
- IPv4 Header
- IPv6 Header
- TCP Header
- UDP Header
- ICMP Header

### Informações de Pacotes

Três estruturas principais:
- `InternetLayerInfo`: dados da camada Internet
- `TransportLayerInfo`: dados da camada de transporte
- `ApplicationLayerInfo`: dados da camada de aplicação

## Segurança e Permissões

⚠️ **IMPORTANTE**: Este programa requer privilégios de root porque:
- Cria raw sockets para captura de pacotes
- Acessa diretamente a interface de rede
- Lê todo o tráfego da interface

### Recomendações:
1. Execute apenas em ambientes controlados (laboratório)
2. Use `sudo` apenas quando necessário
3. Verifique os logs regularmente
4. Não exponha logs com informações sensíveis

## Solução de Problemas

### Erro: "Permission denied"
```bash
# Solução: Execute com sudo
sudo ./network_monitor
```

### Erro: "Error creating raw socket"
```bash
# Verifique se você tem privilégios de root
sudo -v

# Verifique se o módulo de raw socket está disponível
cat /proc/sys/net/ipv4/ip_forward
```

### Erro: "Error getting interface index"
```bash
# Verifique se a interface existe
ip link show tun0

# Se não existir, verifique se o túnel está rodando
ps aux | grep traffic_tunnel
```

### Interface tun0 não aparece
```bash
# Inicie o programa túnel primeiro
sudo ./traffic_tunnel <interface> -s

# Verifique novamente
ip addr show tun0
```

## Limitações Conhecidas

1. **Interface TUN**: O código assume que pacotes da interface TUN têm 4 bytes de cabeçalho antes do IP
2. **Protocolos**: Detecção de protocolos de aplicação baseada em portas conhecidas
3. **IPv6**: Suporte básico, não trata todos os extension headers
4. **Performance**: Para tráfego muito intenso (>10k pps), pode haver perda de pacotes

## Desenvolvimento Futuro

Possíveis melhorias:
- [ ] Suporte para PCAP para replay de tráfego
- [ ] Análise profunda de payloads (DPI)
- [ ] Detecção de anomalias
- [ ] Interface web com gráficos
- [ ] Exportação para formatos adicionais (JSON, XML)
- [ ] Suporte para filtragem de pacotes (BPF)

## Autores

Desenvolvido como trabalho da disciplina de Laboratório de Redes.

## Licença

Este projeto é desenvolvido para fins educacionais.

## Referências

- Stevens, W. Richard. "Unix Network Programming"
- RFC 791 - Internet Protocol (IPv4)
- RFC 793 - Transmission Control Protocol
- RFC 768 - User Datagram Protocol
- RFC 792 - Internet Control Message Protocol
- Linux Kernel Documentation - Networking

## Contato e Suporte

Para dúvidas e problemas:
1. Consulte este README
2. Verifique os logs de erro
3. Execute com opção `-h` para ajuda
4. Contate o professor da disciplina
