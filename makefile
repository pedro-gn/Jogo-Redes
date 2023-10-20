# Compilador e flags
CC = gcc
CFLAGS =  -Iinclude

# Diretórios
SRC_DIR = src
BIN_DIR = bin
INCLUDE_DIR = include

# Alvos padrão
all: servidor cliente

servidor: $(SRC_DIR)/servidor.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $<

cliente: $(SRC_DIR)/cliente.c
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $<

# Limpar arquivos compilados
clean:
	rm -f $(BIN_DIR)/servidor $(BIN_DIR)/cliente
