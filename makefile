CC			= arm-none-eabi-gcc
CFLAGS		= -Wall -march=armv7-m -mtune=cortex-m3 -mthumb -nostartfiles --specs=nano.specs
INCLUDES	= -I inc/
PROG_NAME	= synthlib

SRC_DIR = src
OBJ_DIR = obj

STARTUP_FILE = $(SRC_DIR)/startup.s

STARTUP_OBJ = $(OBJ_DIR)/startup.o

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES)) $(STARTUP_OBJ)

./bin/$(PROG_NAME): $(OBJECTS)
	$(CC) -T linkerscript.ld $(CFLAGS) $^ -o $@ 

$(STARTUP_OBJ): $(STARTUP_FILE)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

clean:
	rm obj/*
	rm bin/*