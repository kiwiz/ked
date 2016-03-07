all: ked.c
	gcc -g -ggdb ked.c term.c sbuf.c sbline.c str.c util.c -std=c11 -o ked
