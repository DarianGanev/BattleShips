CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = battleships
SOURCES = main.c game.c gameplay.c

ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    RM = del
else
    RM = rm -f
endif

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	$(RM) $(TARGET)

.PHONY: clean