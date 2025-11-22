#!/bin/bash
# Script para testar o Monitor de Tráfego de Rede

echo "=== Teste do Monitor de Tráfego de Rede ==="
echo ""

# Verificar se o executável existe
if [ ! -f ./network_monitor ]; then
    echo "✗ Executável não encontrado. Execute 'make' primeiro."
    exit 1
fi

echo "✓ Executável encontrado"

# Verificar privilégios de root
if [ "$EUID" -ne 0 ]; then 
    echo "✗ Este teste precisa ser executado como root (use sudo)"
    exit 1
fi

echo "✓ Executando como root"

# Verificar se a interface tun0 existe
if ! ip link show tun0 &> /dev/null; then
    echo "⚠ Interface tun0 não encontrada"
    echo "  O túnel precisa estar rodando primeiro"
    echo "  Verifique com: ip link show"
    echo ""
    echo "Interfaces disponíveis:"
    ip -br link show
    echo ""
    read -p "Deseja continuar mesmo assim? (s/n) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Ss]$ ]]; then
        exit 1
    fi
else
    echo "✓ Interface tun0 encontrada"
fi

# Verificar diretório de logs
if [ ! -d ./logs ]; then
    echo "Criando diretório de logs..."
    mkdir -p logs
fi

echo "✓ Diretório de logs pronto"

# Executar teste curto
echo ""
echo "Iniciando teste de 10 segundos..."
echo "Pressione Ctrl+C para interromper antes"
echo ""

timeout 10s ./network_monitor -i tun0 -l ./logs || true

echo ""
echo "=== Verificando resultados ==="
echo ""

# Verificar logs
if [ -f ./logs/camada_internet.csv ]; then
    LINES=$(wc -l < ./logs/camada_internet.csv)
    echo "✓ camada_internet.csv criado ($LINES linhas)"
else
    echo "✗ camada_internet.csv não foi criado"
fi

if [ -f ./logs/camada_transporte.csv ]; then
    LINES=$(wc -l < ./logs/camada_transporte.csv)
    echo "✓ camada_transporte.csv criado ($LINES linhas)"
else
    echo "✗ camada_transporte.csv não foi criado"
fi

if [ -f ./logs/camada_aplicacao.csv ]; then
    LINES=$(wc -l < ./logs/camada_aplicacao.csv)
    echo "✓ camada_aplicacao.csv criado ($LINES linhas)"
else
    echo "✗ camada_aplicacao.csv não foi criado"
fi

echo ""
echo "Para ver os logs:"
echo "  cat logs/camada_internet.csv"
echo "  cat logs/camada_transporte.csv"
echo "  cat logs/camada_aplicacao.csv"
echo ""
echo "=== Teste concluído ==="
