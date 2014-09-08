all: logappend logread

logread:
	gcc -g -ggdb ../logReadSrc/* -lssl -lcrypto -o logread

logappend:
	gcc -g -ggdb ../logAppendSrc/* -lssl -lcrypto -o logappend

clean:
	rm logappend
	rm logread
