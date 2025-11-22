# Guia de Integração com o Túnel

## Visão Geral

Este documento descreve como integrar o Monitor de Tráfego de Rede com o sistema de túnel fornecido para o trabalho.

## Arquitetura Completa

```
┌─────────────────────────────────────────────────────────────────────┐
│                            CLIENTES                                  │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  Aplicações → tun0 (172.31.66.x) → Programa Túnel → eth0 → LAN     │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    │ LAN (10.x.x.x)
                                    │
                                    ↓
┌─────────────────────────────────────────────────────────────────────┐
│                        SERVIDOR PROXY                                │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  LAN → eth0 → Programa Túnel → tun0 (172.31.66.1) → NAT/Routing    │
│                                       ↑                              │
│                                       │                              │
│                               [MONITOR DE TRÁFEGO]                   │
│                                       │                              │
│                                       └→ Logs CSV                    │
│                                       └→ Estatísticas                │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
                                    │
                                    ↓
                               INTERNET
```

## Configuração Passo a Passo

### 1. Preparação do Ambiente

#### Requisitos
- Pelo menos 2 máquinas Linux (ou containers)
- Rede local (LAN) conectando as máquinas
- Privilégios de root em ambas
- Fontes do `traffic_tunnel` (fornecido pelo professor)

#### Topologia Sugerida

```
┌──────────────┐         ┌──────────────┐         ┌──────────────┐
│  Cliente 1   │         │  Cliente 2   │         │  Cliente N   │
│ 172.31.66.2  │         │ 172.31.66.3  │         │172.31.66.xxx │
│ eth0: DHCP   │         │ eth0: DHCP   │         │ eth0: DHCP   │
└──────┬───────┘         └──────┬───────┘         └──────┬───────┘
       │                        │                        │
       └────────────────────────┴────────────────────────┘
                                │
                         Switch/LAN (10.x.x.x)
                                │
                    ┌───────────┴────────────┐
                    │   Servidor Proxy       │
                    │   172.31.66.1 (tun0)   │
                    │   10.x.x.x (eth0)      │
                    └───────────┬────────────┘
                                │
                            Internet
```

### 2. Compilação dos Programas

#### No Servidor Proxy

```bash
# 1. Compilar o túnel
cd /path/to/traffic_tunnel
make

# 2. Compilar o monitor
cd /path/to/network_monitor
make

# Verificar
ls -la traffic_tunnel
ls -la network_monitor
```

#### Nos Clientes

```bash
# Compilar apenas o túnel
cd /path/to/traffic_tunnel
make
```

### 3. Configuração do Servidor Proxy

#### Script de Inicialização (server_start.sh)

```bash
#!/bin/bash
# server_start.sh - Inicializa servidor proxy

set -e

INTERFACE="eth0"  # Ajuste conforme sua interface

echo "=== Iniciando Servidor Proxy ==="

# 1. Habilitar IP forwarding
echo 1 > /proc/sys/net/ipv4/ip_forward
echo "✓ IP forwarding habilitado"

# 2. Iniciar túnel
echo "Iniciando túnel na interface $INTERFACE..."
./traffic_tunnel $INTERFACE -s &
TUNNEL_PID=$!
echo "✓ Túnel iniciado (PID: $TUNNEL_PID)"

# Aguardar interface tun0
sleep 2

# 3. Verificar interface tun0
if ip link show tun0 &> /dev/null; then
    echo "✓ Interface tun0 criada"
    ip addr show tun0
else
    echo "✗ Erro: Interface tun0 não foi criada"
    kill $TUNNEL_PID
    exit 1
fi

# 4. Configurar NAT (se necessário)
# Nota: O túnel pode já configurar isso automaticamente
# iptables -t nat -A POSTROUTING -s 172.31.66.0/24 -o eth0 -j MASQUERADE

echo ""
echo "=== Servidor Proxy Pronto ==="
echo "Interface: tun0 (172.31.66.1)"
echo "PID do túnel: $TUNNEL_PID"
echo ""
echo "Para iniciar o monitor:"
echo "  sudo ./network_monitor -i tun0"
```

#### Executar

```bash
chmod +x server_start.sh
sudo ./server_start.sh
```

### 4. Configuração dos Clientes

#### Script de Inicialização (client_start.sh)

```bash
#!/bin/bash
# client_start.sh - Inicializa cliente do túnel

set -e

if [ -z "$1" ]; then
    echo "Uso: $0 <client_script.sh>"
    echo "Exemplo: $0 client1.sh"
    exit 1
fi

CLIENT_SCRIPT=$1
INTERFACE="eth0"  # Ajuste conforme sua interface

echo "=== Iniciando Cliente do Túnel ==="
echo "Script: $CLIENT_SCRIPT"

# Verificar se script existe
if [ ! -f "$CLIENT_SCRIPT" ]; then
    echo "✗ Erro: Script $CLIENT_SCRIPT não encontrado"
    exit 1
fi

# Iniciar túnel
./traffic_tunnel $INTERFACE -c $CLIENT_SCRIPT &
TUNNEL_PID=$!
echo "✓ Túnel iniciado (PID: $TUNNEL_PID)"

# Aguardar interface
sleep 2

# Verificar interface
if ip link show tun0 &> /dev/null; then
    echo "✓ Interface tun0 criada"
    ip addr show tun0
else
    echo "✗ Erro: Interface tun0 não foi criada"
    kill $TUNNEL_PID
    exit 1
fi

echo ""
echo "=== Cliente Pronto ==="
echo "IP do túnel: $(ip addr show tun0 | grep 'inet ' | awk '{print $2}')"
echo "PID do túnel: $TUNNEL_PID"
echo ""
echo "Para testar conectividade:"
echo "  ping 172.31.66.1"
echo "  ping 8.8.8.8"
```

#### Exemplo de client1.sh

```bash
#!/bin/bash
# client1.sh - Configuração do cliente 1

# IP do servidor proxy na LAN
SERVER_IP="10.x.x.x"  # Ajuste para o IP real do servidor

# IP do cliente no túnel
CLIENT_TUNNEL_IP="172.31.66.2"

echo "SERVER_IP=$SERVER_IP"
echo "CLIENT_TUNNEL_IP=$CLIENT_TUNNEL_IP"
```

#### Executar em cada cliente

```bash
chmod +x client_start.sh
chmod +x client1.sh  # client2.sh, etc.

# Cliente 1
sudo ./client_start.sh client1.sh

# Cliente 2 (em outra máquina)
sudo ./client_start.sh client2.sh
```

### 5. Iniciar o Monitor

#### No Servidor Proxy

```bash
# Em um novo terminal
sudo ./network_monitor -i tun0 -l ./logs
```

### 6. Gerar Tráfego para Teste

#### Nos Clientes

```bash
# Teste 1: Ping
ping -c 10 8.8.8.8

# Teste 2: DNS
nslookup google.com

# Teste 3: HTTP
curl http://example.com

# Teste 4: HTTPS
curl https://www.google.com

# Teste 5: Tráfego contínuo
while true; do
    ping -c 1 8.8.8.8
    sleep 1
done
```

### 7. Verificação

#### No Servidor Proxy

```bash
# Terminal 1: Monitor rodando
# Você verá:
# - Contadores atualizando em tempo real
# - Estatísticas por cliente

# Terminal 2: Verificar logs
cat logs/camada_internet.csv | tail -20
cat logs/camada_transporte.csv | tail -20
cat logs/camada_aplicacao.csv | tail -20

# Análise
echo "Total de pacotes IPv4:"
grep "IPv4" logs/camada_internet.csv | wc -l

echo "Tráfego por cliente:"
cut -d',' -f3 logs/camada_internet.csv | grep "172.31.66" | sort | uniq -c
```

## Troubleshooting

### Problema: Interface tun0 não é criada

**Sintomas**: `ip link show tun0` retorna erro

**Soluções**:
```bash
# Verificar módulo TUN/TAP
lsmod | grep tun

# Carregar módulo se necessário
sudo modprobe tun

# Verificar se dispositivo existe
ls -la /dev/net/tun
```

### Problema: Clientes não conseguem acessar internet

**Sintomas**: Ping para 8.8.8.8 falha

**Verificações**:
```bash
# No servidor proxy
# 1. Verificar IP forwarding
cat /proc/sys/net/ipv4/ip_forward  # Deve ser 1

# 2. Verificar roteamento
ip route show

# 3. Verificar iptables
iptables -t nat -L -n -v

# 4. Testar conectividade do servidor
ping 8.8.8.8
```

### Problema: Monitor não captura pacotes

**Sintomas**: Contadores permanecem em zero

**Verificações**:
```bash
# 1. Verificar se túnel está rodando
ps aux | grep traffic_tunnel

# 2. Verificar se tun0 está UP
ip link show tun0 | grep UP

# 3. Capturar com tcpdump para confirmar tráfego
sudo tcpdump -i tun0 -n

# 4. Verificar permissões
ls -la network_monitor

# 5. Executar com sudo
sudo ./network_monitor -i tun0
```

### Problema: Logs não são criados

**Sintomas**: Diretório logs/ está vazio

**Soluções**:
```bash
# Verificar permissões do diretório
ls -la logs/

# Criar manualmente se necessário
mkdir -p logs
chmod 755 logs

# Verificar se o monitor tem permissão de escrita
touch logs/test.txt
rm logs/test.txt
```

## Scripts Úteis

### Monitor de Status (status.sh)

```bash
#!/bin/bash
# status.sh - Verifica status do sistema

echo "=== Status do Sistema de Túnel ==="
echo ""

echo "1. Processos do túnel:"
ps aux | grep traffic_tunnel | grep -v grep

echo ""
echo "2. Interface tun0:"
ip addr show tun0 2>/dev/null || echo "  Interface não encontrada"

echo ""
echo "3. Roteamento:"
ip route | grep tun0

echo ""
echo "4. Monitor:"
ps aux | grep network_monitor | grep -v grep

echo ""
echo "5. Logs recentes:"
if [ -d logs ]; then
    echo "  Última atualização camada_internet: $(stat -c %y logs/camada_internet.csv 2>/dev/null)"
    echo "  Linhas em camada_internet: $(wc -l < logs/camada_internet.csv 2>/dev/null)"
    echo "  Linhas em camada_transporte: $(wc -l < logs/camada_transporte.csv 2>/dev/null)"
    echo "  Linhas em camada_aplicacao: $(wc -l < logs/camada_aplicacao.csv 2>/dev/null)"
fi
```

### Cleanup (cleanup.sh)

```bash
#!/bin/bash
# cleanup.sh - Limpa processos e configurações

echo "=== Limpeza do Sistema ==="

# Parar monitor
killall network_monitor 2>/dev/null && echo "✓ Monitor parado"

# Parar túnel
killall traffic_tunnel 2>/dev/null && echo "✓ Túnel parado"

# Remover interface (será recriada automaticamente)
ip link delete tun0 2>/dev/null && echo "✓ Interface tun0 removida"

echo "Limpeza concluída"
```

## Exemplo Completo de Sessão

```bash
# ========== No Servidor Proxy ==========

# Terminal 1: Iniciar túnel
sudo ./traffic_tunnel eth0 -s

# Terminal 2: Iniciar monitor
sudo ./network_monitor -i tun0 -l ./logs

# Terminal 3: Monitorar logs
watch -n 1 "tail -5 logs/camada_internet.csv"


# ========== No Cliente 1 ==========

# Iniciar túnel
sudo ./traffic_tunnel eth0 -c client1.sh

# Gerar tráfego
ping 8.8.8.8
curl http://example.com
nslookup google.com


# ========== Análise (Servidor) ==========

# Verificar captura
cat logs/camada_internet.csv | grep "172.31.66.2"

# Estatísticas
echo "Pacotes por protocolo:"
cut -d',' -f2 logs/camada_internet.csv | sort | uniq -c

echo "Top destinos:"
cut -d',' -f4 logs/camada_internet.csv | sort | uniq -c | sort -rn | head -10
```

## Considerações Finais

1. **Segurança**: Execute apenas em ambiente de laboratório controlado
2. **Performance**: Para tráfego intenso, considere aumentar BUFFER_SIZE
3. **Logs**: Os arquivos CSV crescem indefinidamente; implemente rotação se necessário
4. **Debugging**: Use tcpdump em paralelo para validar captura

## Referências

- Documentação do traffic_tunnel (fornecida pelo professor)
- `man tun` - Documentação do driver TUN/TAP
- `man iptables` - Configuração de NAT e firewall
