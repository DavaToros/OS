CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -pthread

main: main.c arrays.c parallel.c arrays.h
	$(CC) $(CFLAGS) main.c arrays.c parallel.c -o main

clean:
	rm -f main

THREADS ?= 14

stats: main
	@./main -k 64 -n 100000 -t $(THREADS) -s & \
	pid=$$!; sleep 2; ps -M -p $$pid; wait $$pid

.PHONY: clean stats
