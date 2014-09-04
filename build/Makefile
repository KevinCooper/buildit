all: logappend logread

logread:
	gcc -Ofast ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -Ofast  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
