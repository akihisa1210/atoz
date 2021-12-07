CC = gcc
CFLAGS = -g -Wall

atoz: atoz.c
	$(CC) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf atoz
