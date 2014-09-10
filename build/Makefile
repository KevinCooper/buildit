all: logappend logread

logread:
	gcc -march=native -g -ggdb  ../logReadSrc/* -lssl -lcrypto -lrt -o logread

logappend:
	gcc -march=native -g -ggdb ../logAppendSrc/* -lssl -lcrypto -lrt -o logappend

clean:
	rm logappend
	rm logread
