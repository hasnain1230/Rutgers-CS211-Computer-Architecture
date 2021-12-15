TARGET = cachesim
CC     = gcc
CFLAGS = -g -std=c99 -Wall -Wvla -Werror -O3 -fsanitize=address,undefined

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET) *.o *.a *.dylib *.dSYM
