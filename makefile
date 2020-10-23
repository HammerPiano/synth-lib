CC			= arm-none-eabi-gcc
OBJ_COPY	= arm-none-eabi-objcopy
FLASH		= st-flash
FLASH_SIZE 	= --flash=64k
FLASH_OFFSET= 0x08000000
CFLAGS		= -Wall -march=armv7-m -mthumb -nostartfiles --specs=nosys.specs -ffunction-sections -fdata-sections -masm-syntax-unified -mlittle-endian -g
INCLUDES	= -I inc/ -I inc/core
PROG_NAME	= synthlib

SRC_DIR = src
OBJ_DIR = obj

STARTUP_FILE = $(SRC_DIR)/startup.s

STARTUP_OBJ = $(OBJ_DIR)/startup.o

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES)) $(STARTUP_OBJ)

all: ./bin/$(PROG_NAME)

flash: 
	$(FLASH) $(FLASH_SIZE) write ./bin/$(PROG_NAME) $(FLASH_OFFSET)


./bin/$(PROG_NAME): ./bin/$(PROG_NAME).elf
	$(OBJ_COPY) -O binary $^ $@

./bin/$(PROG_NAME).elf: $(OBJECTS)
	$(CC) -T linkerscript.ld $(CFLAGS) $^ -o $@ 

$(STARTUP_OBJ): $(STARTUP_FILE)
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

clean:
	rm obj/*
	rm bin/*