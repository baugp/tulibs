HAVE_LIBGCC_S = 1

CFLAGS += -I../timer -fPIC
LDFLAGS += -L../timer -lpthread -ltimer

ifeq ($(HAVE_LIBGCC_S),1)
	CFLAGS += -DHAVE_LIBGCC_S
endif

PREFIX = /usr/local
OBJECTS = thread.o mutex.o

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

libthread.so: $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -o libthread.so $(OBJECTS)

install: libthread.so
	install -c -m 755 libthread.so $(PREFIX)/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
