MCU ?= atmega328p
F_CPU ?= 16000000UL
BAUD ?= 9600
TEST ?= false
CORE ?= true


ifeq ($(TEST), true)
	CC = gcc
	CXX = g++
	CXXFLAGS = -Os -Wall
	MN_FILE = main
	LINK_FLAGS = -o $(MN_FILE)
else
	CC = avr-gcc
	CXX = avr-g++
	CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall
	MN_FILE = main.elf
	LINK_FLAGS = -mmcu=$(MCU) -o $(MN_FILE)
endif

ifeq ($(CORE), true)
	CXXFLAGS += -I../arduino-core
	LINK_FLAGS += ../arduino-core/*.o
endif

INO_NAME = src/src
$(shell cp $(INO_NAME).ino $(INO_NAME).cpp)
SRCS ?= $(wildcard src/Drivers/*.cpp) $(wildcard src/*.cpp) $(wildcard src/States/*.cpp)
TOTAL_SRCS += $(SRCS)
OBJS = $(TOTAL_SRCS:.cpp=.o)
PORT ?= /dev/ttyUSB0
BAUD ?= 9600


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(MN_FILE): $(OBJS)
	$(CC) $(LINK_FLAGS) $(OBJS)
	avr-size $(MN_FILE)

.PHONY: all install clean monitor

all: $(MN_FILE)

install: $(MN_FILE)
	avr-objcopy -O ihex $(MN_FILE) main.hex
	sudo avrdude -c arduino -p m328p -C /etc/avrdude.conf -P $(PORT) -U flash:w:main.hex
	minicom --dev $(PORT) -b $(BAUD)

monitor:
	minicom --dev $(PORT) -b $(BAUD)

clean:
	rm -f $(OBJS) *.elf *.hex

