SRC_PATH = ./source
SRC = $(shell find $(SRC_PATH) -name '*.c')
PRG = $(SRC:$(SRC_PATH)/%.c=%)

all: build exec clear

build: $(PRG)
	gcc main.c -o main

exec: 
	./main

clear:
	rm $(PRG)
	rm main

%: $(SRC_PATH)/%.c
	gcc $< -o $@