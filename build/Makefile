all: logappend logread

logread:
	gcc -march=native -Ofast ../logReadSrc/* -lssl -lcrypto -o logread

logappend:
	gcc -march=native -Ofast ../logAppendSrc/* -lssl -lcrypto -o logappend

clean:
	rm logappend
	rm logread
