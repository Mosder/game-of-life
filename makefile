main: main.c lib-bitmap lib-setup lib-render
	gcc main.c libbitmap.a libsetup.a librender.a -o main
	rm libbitmap.a libsetup.a librender.a

lib-bitmap: libs/bitmap/bitmap.c
	gcc -c libs/bitmap/bitmap.c
	ar rcs libbitmap.a bitmap.o
	rm bitmap.o

lib-setup: libs/setup/setup.c
	gcc -c libs/setup/setup.c
	ar rcs libsetup.a setup.o
	rm setup.o

lib-render: libs/render/render.c
	gcc -c libs/render/render.c
	ar rcs librender.a render.o
	rm render.o

.PHONY: clean
clean:
	rm main

