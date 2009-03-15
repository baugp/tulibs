CFLAGS += -fPIC
LDFLAGS +=

PREFIX = /usr/local

libtimer.so: timer.h timer.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libtimer.so timer.c

install: libtimer.so
	install -c -m 755 libtimer.so $(PREFIX)/lib

clean:
	rm -f *.o *.a *.so
