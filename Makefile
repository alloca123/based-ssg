PREFIX ?= /usr/local
CC ?= cc
INSTALL ?= install

all:
	$(CC) based-ssg.c libsmu.c -o based-ssg -march=native -std=c89 -O2 $(CFLAGS)
install:
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) based-ssg $(PREFIX)/bin/
clean:
	rm -f based-ssg
	rm -f examples/example.html
	rm -f examples/example2.html
	rm -f examples/rss.xml
uninstall:
	rm -f $(PREFIX)/bin/based-ssg
