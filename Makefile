CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -Wpedantic -O2
TARGET  := tictactoe

.PHONY: all run test clean

all: $(TARGET)

$(TARGET): tictactoe.c
	$(CC) $(CFLAGS) -o $@ $<

run: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) --selftest

clean:
	rm -f $(TARGET) $(TARGET).exe
