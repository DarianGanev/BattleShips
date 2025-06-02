# Makefile for Battle Ships project

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = battleships
SOURCES = main.c game.c gameplay.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	del $(TARGET).exe

.PHONY: clean
