CFLAGS += -I../timer -fPIC
LDFLAGS += -L../timer -lpthread -ltimer

PREFIX = /usr/local

libthread.so: thread.h thread.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libthread.so thread.c

install: libthread.so
	install -c -m 755 libthread.so $(PREFIX)/lib

clean:
	rm -f *.o *.a *.so
