all:
	pcc based-ssg.c libsmu.c -o based-ssg
clean:
	rm -f *.6
	rm -f based-ssg
