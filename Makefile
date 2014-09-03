all: logappend logread

logread:
	gcc -g -ggdb -lssl -lcrypto logReadSrc/* -o build/logread

logappend:
	gcc -g -ggdb -lssl -lcrypto logAppendSrc/* -o build/logappend
