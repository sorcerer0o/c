SRCS := hashmap.c main.c
OUT  := hashmap
CC   := gcc
CFLAGS := -Wall -g

.PHONY: clean rebuild all

all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(SRCS) -o $(OUT) $(CFLAGS)

clean:
	$(RM) $(OUT)

rebuild: clean all
