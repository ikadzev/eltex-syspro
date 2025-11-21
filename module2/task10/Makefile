LIB_PATH = ./lib
SRC_PATH = ./source
OBJ_PATH = ./obj
SRCS = $(shell find $(SRC_PATH) -name '*.c')
OBJS = $(SRCS:$(SRC_PATH)/%.c=$(OBJ_PATH)/%.o)
LIBS = $(OBJS:$(OBJ_PATH)/%.o=$(LIB_PATH)/%.so)


.PHONY: exec
exec: $(LIBS)
	gcc -rdynamic main.c -o main -ldl
	./main

$(LIB_PATH)/%.so: $(OBJ_PATH)/%.o
	gcc -shared $< -o $@

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	gcc $< -fPIC -c -o $@
	