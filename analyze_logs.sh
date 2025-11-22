#!/bin/bash
# analyze_logs.sh - Script para análise dos logs CSV

LOG_DIR="./logs"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║        ANÁLISE DE LOGS - Monitor de Tráfego                ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

if [ ! -d "$LOG_DIR" ]; then
    echo "✗ Erro: Diretório de logs não encontrado: $LOG_DIR"
    exit 1
fi

# Verificar se logs existem
if [ ! -f "$LOG_DIR/camada_internet.csv" ]; then
    echo "✗ Erro: Logs não encontrados. Execute o monitor primeiro."
    exit 1
fi

echo "1. ESTATÍSTICAS GERAIS"
echo "─────────────────────────────────────────────────────────────"

# Total de pacotes por camada
INET_LINES=$(($(wc -l < "$LOG_DIR/camada_internet.csv") - 1))
TRANS_LINES=$(($(wc -l < "$LOG_DIR/camada_transporte.csv") - 1))
APP_LINES=$(($(wc -l < "$LOG_DIR/camada_aplicacao.csv") - 1))

echo "Total de pacotes capturados:"
echo "  Camada Internet:   $INET_LINES"
echo "  Camada Transporte: $TRANS_LINES"
echo "  Camada Aplicação:  $APP_LINES"
echo ""

echo "2. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA INTERNET"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f2 | sort | uniq -c | sort -rn
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "3. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA TRANSPORTE"
echo "─────────────────────────────────────────────────────────────"
if [ $TRANS_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_transporte.csv" | cut -d',' -f2 | sort | uniq -c | sort -rn
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "4. DISTRIBUIÇÃO DE PROTOCOLOS - CAMADA APLICAÇÃO"
echo "─────────────────────────────────────────────────────────────"
if [ $APP_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_aplicacao.csv" | cut -d',' -f2 | sort | uniq -c | sort -rn
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "5. TOP 10 IPs ORIGEM"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f3 | sort | uniq -c | sort -rn | head -10
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "6. TOP 10 IPs DESTINO"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f4 | sort | uniq -c | sort -rn | head -10
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "7. TOP 10 PORTAS DESTINO"
echo "─────────────────────────────────────────────────────────────"
if [ $TRANS_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_transporte.csv" | cut -d',' -f6 | sort | uniq -c | sort -rn | head -10
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "8. CLIENTES DO TÚNEL ATIVOS (172.31.66.x)"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f3 | grep "^172\.31\.66\." | sort -u
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "9. TRÁFEGO POR CLIENTE (Pacotes)"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    echo "Cliente IP         | Origem | Destino"
    echo "───────────────────┼────────┼─────────"
    for client_ip in $(tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f3 | grep "^172\.31\.66\." | sort -u); do
        OUTBOUND=$(tail -n +2 "$LOG_DIR/camada_internet.csv" | grep "^[^,]*,$client_ip," | wc -l 2>/dev/null || echo 0)
        INBOUND=$(tail -n +2 "$LOG_DIR/camada_internet.csv" | grep ",[^,]*,$client_ip," | wc -l 2>/dev/null || echo 0)
        printf "%-18s | %6d | %7d\n" "$client_ip" "$OUTBOUND" "$INBOUND"
    done
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "10. SERVIÇOS MAIS ACESSADOS"
echo "─────────────────────────────────────────────────────────────"
if [ $APP_LINES -gt 0 ]; then
    echo "Protocolo | Acessos"
    echo "──────────┼─────────"
    tail -n +2 "$LOG_DIR/camada_aplicacao.csv" | cut -d',' -f2 | sort | uniq -c | sort -rn | while read count proto; do
        printf "%-9s | %7d\n" "$proto" "$count"
    done
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "11. ANÁLISE TEMPORAL (Últimos 10 pacotes)"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    echo "Timestamp           | Protocolo | Origem → Destino"
    echo "────────────────────┼───────────┼─────────────────────────"
    tail -10 "$LOG_DIR/camada_internet.csv" | while IFS=',' read timestamp proto src dst rest; do
        printf "%-19s | %-9s | %s → %s\n" "$timestamp" "$proto" "$src" "$dst"
    done
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "12. ESTATÍSTICAS DE TAMANHO DE PACOTES"
echo "─────────────────────────────────────────────────────────────"
if [ $INET_LINES -gt 0 ]; then
    SIZES=$(tail -n +2 "$LOG_DIR/camada_internet.csv" | cut -d',' -f7)
    
    # Calcular estatísticas básicas usando awk
    echo "$SIZES" | awk '
    BEGIN {
        min = 999999; max = 0; sum = 0; count = 0;
    }
    {
        if ($1 < min) min = $1;
        if ($1 > max) max = $1;
        sum += $1;
        count++;
    }
    END {
        if (count > 0) {
            printf "  Mínimo:  %d bytes\n", min;
            printf "  Máximo:  %d bytes\n", max;
            printf "  Média:   %.2f bytes\n", sum/count;
            printf "  Total:   %.2f KB\n", sum/1024;
        }
    }'
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "13. CONEXÕES POR CLIENTE (Camada Transporte)"
echo "─────────────────────────────────────────────────────────────"
if [ $TRANS_LINES -gt 0 ]; then
    echo "Cliente          | Destino          | Porta | Protocolo | Count"
    echo "─────────────────┼──────────────────┼───────┼───────────┼──────"
    tail -n +2 "$LOG_DIR/camada_transporte.csv" | \
        awk -F',' '
        $3 ~ /^172\.31\.66\./ {
            key = $3 " | " $5 " | " $6 " | " $2;
            count[key]++;
        }
        END {
            for (k in count) {
                print k " | " count[k];
            }
        }' | sort -t'|' -k5 -rn | head -20
else
    echo "  Nenhum dado disponível"
fi
echo ""

echo "╔════════════════════════════════════════════════════════════╗"
echo "║                   FIM DA ANÁLISE                           ║"
echo "╚════════════════════════════════════════════════════════════╝"
