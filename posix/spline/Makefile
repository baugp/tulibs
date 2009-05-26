CFLAGS += -fPIC
LDFLAGS += 

PREFIX = /usr/local
OBJECTS = spline.o

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

libspline.so: $(OBJECTS)
	$(CC) $(LDFLAGS) -shared -o libspline.so $(OBJECTS)

install: libspline.so
	install -c -m 755 libspline.so $(PREFIX)/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
