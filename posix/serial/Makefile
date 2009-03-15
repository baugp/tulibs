CFLAGS += -fPIC
LDFLAGS += -lm

PREFIX = /usr/local

libserial.so: serial.h serial.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libserial.so serial.c

install: libserial.so
	install -c -m 755 libserial.so $(PREFIX)/lib

clean:
	rm -f *.o *.a *.so
