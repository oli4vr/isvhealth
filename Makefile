CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lcurl -lcjson

TARGET = isvhealth
SOURCES = main.c http_client.c json_parser.c auth.c health.c performance.c output.c
HEADERS = http_client.h json_parser.h auth.h health.h performance.h output.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

install:
	install -m 755 $(TARGET) /usr/local/bin/

.PHONY: all clean install
