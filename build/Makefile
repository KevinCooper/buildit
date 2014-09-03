all: logappend logread

logread:
	gcc -g -ggdb -lssl -lcrypto ../logReadSrc/* -o logread

logappend:
	gcc -g -ggdb -lssl -lcrypto ../logAppendSrc/* -o logappend

clean:
	rm logread
	rm logappend
