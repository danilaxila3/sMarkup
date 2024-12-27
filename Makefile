all:
	gcc src/smarkup.c $(shell pkgconf --cflags --libs ncursesw) -o bin/smarkup
