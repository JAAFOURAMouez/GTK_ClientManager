CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk4` `pkg-config --cflags cairo`
LDFLAGS = `pkg-config --libs gtk4` `pkg-config --libs cairo`

# Définition des répertoires
SRC_DIR = src
OBJ_DIR = obj
DATA_DIR = data

# Obtenir le chemin absolu du projet
PROJ_DIR := $(shell pwd)

# Ajouter le chemin du répertoire data aux flags de compilation
CFLAGS += -DDATA_DIR=\"$(PROJ_DIR)/$(DATA_DIR)/\"

# Sources et objets
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

EXEC = gestion_bancaire

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXEC)