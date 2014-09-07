all: logappend logread

logread:
	gcc -Ofast -march=native ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -Ofast -march=native  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
