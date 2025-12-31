CC      := gcc
CFLAGS  := -g3 -Wall -Wextra -Werror -Iheaders
TARGET  := main

SRCS    := main.c $(wildcard src/*.c)
OBJS    := $(SRCS:.c=.o)

.PHONY: all clean memcheck run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

memcheck: $(TARGET)
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         ./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)
