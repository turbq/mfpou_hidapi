SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

EXE := $(BIN_DIR)/brightness

SRC := $(wildcard $(SRC_DIR)/*.c)

OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

INCLUDES := `pkg-config hidapi-hidraw --cflags` `pkg-config libusb --cflags`
CPPFLAGS := $(INCLUDES) -Iinclude -MMD -MP # -I is a preprocessor flag, not a compiler flag
CFLAGS   := -Wall -g -fpic     
LDFLAGS  := -Wall -g 			# -Llib
LDLIBS   := `pkg-config libudev --libs` -lhidapi-hidraw

CC = gcc

.PHONY: all clean libs

all: $(EXE)

libs: libhidapi-hidraw.so

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

libhidapi-hidraw.so:
	$(CC) $(LDFLAGS) $(LDLIBS) -shared -fpic -Wl,-soname,$@.0 $^ -o $@

clean:
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR) libhidapi-hidraw.so

#-include $(OBJ:.o=.d)
