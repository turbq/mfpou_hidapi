CROSS_COMPILE = armv7a-linux-gnueabihf-gcc
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/brightness

SRC := $(wildcard $(SRC_DIR)/*.c)

OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

INCLUDES := 
CPPFLAGS := $(INCLUDES) -Iinclude -MMD -MP # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall -g
LDFLAGS  := -Wall -g 			# -Llib
LDLIBS   := 
#`pkg-config libudev --libs`

CC = gcc

.PHONY: all clean

all: $(EXE)

arm: CC = $(CROSS_COMPILE)
arm: BIN_DIR = arm
arm: EXE = $(BIN_DIR)/brightness
arm: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

#-include $(OBJ:.o=.d)
