CC=gcc
CFLAGS=-c -Wall
LDFLAGS=-lfcgi -lcurl -lm
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=binary/hados.fcgi

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm $(EXECUTABLE) src/*.o

distclean: clean
