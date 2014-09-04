all: logappend logread

logread:
	gcc -g -ggdb -march=native ../logReadSrc/* -o logread -lssl -lcrypto

logappend:
	gcc -g -ggdb -march=native  ../logAppendSrc/* -o logappend -lssl -lcrypto

clean:
	rm logappend
	rm logread
