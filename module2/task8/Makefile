all:
	gcc phonebook.c -c
	ar rc libphonebook.a phonebook.o
	rm phonebook.o
	gcc main.c -o main -L. -lphonebook
	./main
clear:
	rm main libphonebook.a