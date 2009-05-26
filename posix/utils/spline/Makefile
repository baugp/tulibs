CFLAGS += -I../../spline
LDFLAGS += -L../../spline -lspline

PREFIX = /usr/local

OBJS = spline_print spline_evaluate

all: $(OBJS)

install: $(OBJS)
	install -c -m 755 $(OBJS) $(PREFIX)/bin

clean:
	rm -f *.o $(OBJS)
