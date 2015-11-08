all: ked.c
	gcc -g -ggdb ked.c str.c -std=c11 -o ked
