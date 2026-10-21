CC      ?= gcc
CFLAGS  ?= -O2
LDFLAGS ?=
# Kept apart so the spec's CFLAGS/LDFLAGS can't drop them.
SO_CFLAGS  := -Wall -Wextra -fPIC
SO_LDFLAGS := -shared -ldl

TARGET  := build/libsteam-stream-colorspace.so

PREFIX  ?= /usr
LIBDIR  ?= $(PREFIX)/lib64
DESTDIR ?=

.PHONY: all clean install

all: $(TARGET)

$(TARGET): src/steam-stream-colorspace.c
	@mkdir -p build
	$(CC) $(SO_CFLAGS) $(CFLAGS) -o $@ $< $(SO_LDFLAGS) $(LDFLAGS)

clean:
	rm -rf build

install: all
	install -Dm755 $(TARGET) $(DESTDIR)$(LIBDIR)/armada/libsteam-stream-colorspace.so
