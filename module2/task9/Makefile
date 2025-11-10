all:
	gcc -fPIC phonebook.c -c
	gcc -shared phonebook.o -o libphonebook.so
	rm phonebook.o
	gcc main.c -o main -L. -lphonebook
	LD_LIBRARY_PATH=$$PWD:$$LD_LIBRARY_PATH ./main
clear:
	rm main libphonebook.so