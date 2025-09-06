CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0 gtk-layer-shell-0) $(shell pkg-config --cflags libcjson) -g
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 gtk-layer-shell-0) $(shell pkg-config --libs libcjson)
TARGET = run
SRC = test.c do_bash.c
OBJ = $(SRC:.c=.o)
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

test: $(TARGET)
	./$(TARGET)

-include $(DEP)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

.PHONY:
	all clean
