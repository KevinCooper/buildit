all: logappend logread

logread:
	gcc -ggdb -g ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -ggdb -g  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
