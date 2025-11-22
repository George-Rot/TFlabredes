#!/bin/bash

echo "=== Teste de Captura ICMP ==="
echo ""
echo "Este script irá:"
echo "1. Iniciar o monitor em background"
echo "2. Gerar tráfego ICMP (ping)"
echo "3. Aguardar 10 segundos"
echo "4. Parar o monitor"
echo "5. Mostrar os logs capturados"
echo ""
echo "Pressione ENTER para continuar..."
read

# Limpa logs antigos
echo "Limpando logs antigos..."
rm -f logs/*.csv

# Inicia o monitor em background
echo "Iniciando monitor..."
sudo ./network_monitor &
MONITOR_PID=$!
echo "Monitor iniciado com PID $MONITOR_PID"
sleep 2

# Gera tráfego ICMP
echo ""
echo "Gerando tráfego ICMP (ping)..."
ping -c 5 8.8.8.8 > /dev/null 2>&1 &
ping -c 5 1.1.1.1 > /dev/null 2>&1 &

# Aguarda um pouco
echo "Aguardando captura de pacotes..."
sleep 10

# Para o monitor
echo ""
echo "Parando monitor..."
sudo kill -SIGINT $MONITOR_PID
sleep 2

# Mostra os resultados
echo ""
echo "=== RESULTADOS ==="
echo ""

if [ -f logs/camada_internet.csv ]; then
    echo ">>> Camada Internet (primeiras 10 linhas):"
    head -n 10 logs/camada_internet.csv
    echo ""
    ICMP_INTERNET=$(grep -c "ICMP" logs/camada_internet.csv || echo "0")
    echo "Total de pacotes ICMP na camada Internet: $ICMP_INTERNET"
else
    echo "Arquivo camada_internet.csv não encontrado!"
fi

echo ""

if [ -f logs/camada_transporte.csv ]; then
    echo ">>> Camada Transporte (primeiras 10 linhas):"
    head -n 10 logs/camada_transporte.csv
    echo ""
    ICMP_TRANSPORT=$(grep -c "ICMP" logs/camada_transporte.csv || echo "0")
    echo "Total de pacotes ICMP na camada Transporte: $ICMP_TRANSPORT"
else
    echo "Arquivo camada_transporte.csv não encontrado!"
fi

echo ""
echo "=== FIM DO TESTE ==="
