# Makefile para Monitor de Tráfego de Rede

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lpthread

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = .
LOG_DIR = logs

# Target executable
TARGET = $(BIN_DIR)/network_monitor

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Default target
all: directories $(TARGET)

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(LOG_DIR)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build successful! Executable: $(TARGET)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)
	@echo "Clean complete"

# Clean logs
clean-logs:
	rm -f $(LOG_DIR)/*.csv
	@echo "Logs cleaned"

# Clean everything
cleanall: clean clean-logs
	@echo "All clean"

# Run the program (requires root/sudo)
run: $(TARGET)
	@echo "Running network monitor (requires root privileges)..."
	sudo $(TARGET)

# Install dependencies (for Debian/Ubuntu)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential

# Help target
help:
	@echo "Monitor de Tráfego de Rede - Makefile"
	@echo ""
	@echo "Targets disponíveis:"
	@echo "  make           - Compila o projeto"
	@echo "  make clean     - Remove arquivos de compilação"
	@echo "  make clean-logs - Remove arquivos de log"
	@echo "  make cleanall  - Remove tudo (compilação e logs)"
	@echo "  make run       - Compila e executa (requer sudo)"
	@echo "  make install-deps - Instala dependências (Ubuntu/Debian)"
	@echo "  make help      - Mostra esta mensagem"
	@echo ""
	@echo "Uso:"
	@echo "  sudo ./network_monitor -i tun0 -l ./logs"

.PHONY: all clean clean-logs cleanall run directories install-deps help
