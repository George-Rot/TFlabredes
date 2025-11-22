#!/bin/bash
# Script de instalação e configuração do Monitor de Tráfego de Rede

set -e

echo "=== Instalação do Monitor de Tráfego de Rede ==="
echo ""

# Verificar se está rodando como root
if [ "$EUID" -ne 0 ]; then 
    echo "AVISO: Este script precisa de privilégios de root para algumas operações."
    echo "Você pode precisar executar 'sudo $0'"
fi

# Verificar sistema operacional
if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS=$NAME
    echo "Sistema operacional detectado: $OS"
else
    echo "AVISO: Não foi possível detectar o sistema operacional"
fi

# Instalar dependências
echo ""
echo "Instalando dependências..."
if command -v apt-get &> /dev/null; then
    apt-get update
    apt-get install -y build-essential gcc make
elif command -v yum &> /dev/null; then
    yum install -y gcc gcc-c++ make
elif command -v dnf &> /dev/null; then
    dnf install -y gcc gcc-c++ make
else
    echo "AVISO: Gerenciador de pacotes não suportado. Instale manualmente: build-essential, gcc, make"
fi

# Criar diretórios necessários
echo ""
echo "Criando diretórios..."
mkdir -p logs
mkdir -p obj

# Compilar o projeto
echo ""
echo "Compilando o projeto..."
make clean
make

# Verificar compilação
if [ -f ./network_monitor ]; then
    echo ""
    echo "✓ Compilação bem-sucedida!"
    echo ""
    echo "Para executar o monitor:"
    echo "  sudo ./network_monitor"
    echo ""
    echo "Para executar com opções:"
    echo "  sudo ./network_monitor -i tun0 -l ./logs"
    echo ""
    echo "Para ajuda:"
    echo "  ./network_monitor -h"
else
    echo ""
    echo "✗ Erro na compilação!"
    exit 1
fi

echo ""
echo "=== Instalação concluída ==="
