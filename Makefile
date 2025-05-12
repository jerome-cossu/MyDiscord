CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lws2_32

SRC_SERVER = server.c
SRC_CLIENT = client.c

OUT_SERVER = server.exe
OUT_CLIENT = client.exe

.PHONY: all clean

all: $(OUT_SERVER) $(OUT_CLIENT)

$(OUT_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

$(OUT_CLIENT): $(SRC_CLIENT)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	del /Q *.exe 2>nul || true
