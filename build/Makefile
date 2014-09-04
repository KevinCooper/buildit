all: logappend logread

logread:
	gcc -g -ggdb ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -g -ggdb  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
