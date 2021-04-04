all:
	cc based-ssg.c libsmu.c -o based-ssg -march=native -std=c89 -O2
install:
	cp based-ssg /usr/local/bin
clean:
	rm based-ssg
uninstall:
	rm /usr/local/bin/based-ssg
