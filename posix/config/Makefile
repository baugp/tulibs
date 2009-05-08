CFLAGS += -fPIC
LDFLAGS += 

PREFIX = /usr/local
OBJECTS = param.o config.o

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

libconfig.so: $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -o libconfig.so $(OBJECTS)

install: libconfig.so
	install -c -m 755 libconfig.so $(PREFIX)/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
