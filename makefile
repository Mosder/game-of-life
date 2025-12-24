main: main.c lib-bitmap lib-render
	gcc main.c libbitmap.a librender.a -o main
	rm libbitmap.a librender.a

lib-bitmap: bitmap.c
	gcc -c bitmap.c
	ar rcs libbitmap.a bitmap.o
	rm bitmap.o

lib-render: render.c
	gcc -c render.c
	ar rcs librender.a render.o
	rm render.o

.PHONY: clean
clean:
	rm main

