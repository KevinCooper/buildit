all: logappend logread

logread:
	gcc -O3 -march=native ../logReadSrc/* -lssl -lcrypto -o logread

logappend:
	gcc -O3 -march=native  ../logAppendSrc/* -lssl -lcrypto -o logappend

clean:
	rm logappend
	rm logread
