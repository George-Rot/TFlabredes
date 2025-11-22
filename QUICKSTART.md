# 🚀 Quick Start - Monitor de Tráfego de Rede

## ⚡ Início Rápido (5 minutos)

### Passo 1: Compilar
```bash
cd /caminho/do/projeto
make
```

### Passo 2: Executar (requer sudo)
```bash
sudo ./network_monitor
```

**Pronto!** O monitor está capturando pacotes na interface tun0.

---

## 📋 Pré-requisitos

### Sistema
- ✅ Linux (Ubuntu, Debian, ou similar)
- ✅ Privilégios root/sudo
- ✅ Interface tun0 configurada (túnel rodando)

### Dependências
```bash
# Instalar automaticamente
./install.sh

# OU manualmente
sudo apt-get update
sudo apt-get install build-essential gcc make
```

---

## 🎯 Cenários de Uso

### Cenário 1: Ambiente de Laboratório com Túnel

#### No Servidor Proxy:
```bash
# Terminal 1: Iniciar túnel
cd /caminho/do/traffic_tunnel
sudo ./traffic_tunnel eth0 -s

# Terminal 2: Iniciar monitor
cd /caminho/do/network_monitor
sudo ./network_monitor -i tun0 -l ./logs
```

#### Nos Clientes:
```bash
# Iniciar túnel do cliente
sudo ./traffic_tunnel eth0 -c client1.sh

# Gerar tráfego
ping 8.8.8.8
curl http://example.com
curl https://www.google.com
```

### Cenário 2: Teste Rápido
```bash
# Compilar e testar por 10 segundos
./test.sh
```

### Cenário 3: Análise de Logs Existentes
```bash
# Analisar logs após captura
./analyze_logs.sh
```

---

## 📊 Visualizando Resultados

### Durante a Execução
O monitor mostra em tempo real:
- Contadores de protocolos
- Estatísticas por cliente
- Pacotes enviados/recebidos

### Após a Execução
```bash
# Ver logs
cat logs/camada_internet.csv
cat logs/camada_transporte.csv
cat logs/camada_aplicacao.csv

# Análise detalhada
./analyze_logs.sh

# Monitoramento contínuo
tail -f logs/camada_internet.csv
```

---

## 🛠️ Comandos Úteis

### Verificar se tudo está OK
```bash
# Interface tun0 existe?
ip link show tun0

# Túnel rodando?
ps aux | grep traffic_tunnel

# Monitor rodando?
ps aux | grep network_monitor

# Tráfego passando?
sudo tcpdump -i tun0 -c 10
```

### Parar tudo
```bash
# Parar monitor (Ctrl+C no terminal)
# OU
sudo killall network_monitor

# Parar túnel
sudo killall traffic_tunnel
```

### Limpar logs
```bash
make clean-logs
```

---

## 📖 Documentação Completa

| Arquivo | Conteúdo |
|---------|----------|
| **README.md** | Documentação principal completa |
| **PROJECT_SUMMARY.md** | Visão geral do projeto |
| **docs/TECHNICAL.md** | Detalhes técnicos e arquitetura |
| **docs/TUNNEL_INTEGRATION.md** | Como integrar com o túnel |
| **docs/EXAMPLES.md** | Exemplos de saída e análises |

---

## ❓ Problemas Comuns

### ❌ "Error creating raw socket"
```bash
# Solução: Execute com sudo
sudo ./network_monitor
```

### ❌ "Error getting interface index"
```bash
# Solução: Interface não existe, inicie o túnel primeiro
sudo ./traffic_tunnel eth0 -s

# Ou especifique outra interface
sudo ./network_monitor -i eth0
```

### ❌ Monitor não captura nada
```bash
# Verifique se há tráfego na interface
sudo tcpdump -i tun0

# Se não houver, gere tráfego de um cliente
ping 8.8.8.8
```

### ❌ "Permission denied" nos logs
```bash
# Criar diretório de logs
mkdir -p logs
chmod 755 logs
```

---

## 🎓 Próximos Passos

1. ✅ **Compilou e executou**: Parabéns!
2. 📚 **Leia o README.md**: Entenda todas as funcionalidades
3. 🧪 **Gere tráfego**: Use ping, curl, wget
4. 📊 **Analise logs**: Execute `./analyze_logs.sh`
5. 🔧 **Personalize**: Modifique o código conforme necessário
6. 📝 **Documente**: Adicione suas observações

---

## 💡 Dicas Importantes

1. **Sempre use sudo**: Raw sockets requerem privilégios
2. **Túnel primeiro**: Inicie o túnel antes do monitor
3. **Logs crescem**: Implemente rotação se necessário
4. **Performance**: Para tráfego intenso, ajuste BUFFER_SIZE
5. **Debugging**: Use tcpdump em paralelo para validar

---

## 🎯 Checklist de Validação

- [ ] Compilou sem erros
- [ ] Executou com sudo
- [ ] Interface tun0 detectada
- [ ] Contadores atualizando
- [ ] Logs sendo criados
- [ ] Estatísticas por cliente aparecem
- [ ] Pode interromper com Ctrl+C
- [ ] Logs contêm dados válidos

---

## 📞 Ajuda

Se encontrar problemas:

1. Leia a seção de Troubleshooting no README.md
2. Execute `./network_monitor -h` para ver opções
3. Verifique os logs de erro no terminal
4. Use `./test.sh` para teste automatizado
5. Consulte docs/TECHNICAL.md para detalhes

---

## 🎉 Sucesso!

Se chegou até aqui e está capturando pacotes:
- ✅ Projeto funcionando
- ✅ Logs sendo gerados
- ✅ Estatísticas em tempo real
- ✅ Pronto para análise!

**Explore os arquivos de documentação para entender melhor o sistema!**

---

📅 Última atualização: 2024-11-22  
💻 Linguagem: C  
📦 Sistema: Linux  
🔒 Requer: sudo
