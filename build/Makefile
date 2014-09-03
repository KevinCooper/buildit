all: logappend logread

logread:
	gcc -O3 ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -O3  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
