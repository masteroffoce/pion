CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0 gtk-layer-shell-0)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 gtk-layer-shell-0)
TARGET = run
SRC = test.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)


clean:
	rm -f $(TARGET)

.PHONY:
	all clean
