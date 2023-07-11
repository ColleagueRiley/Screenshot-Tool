SOURCES = source/main.c single-headers.o
ARGS = -lm -I./include -I./include/deps


all:
	make single-headers.o
	
	gcc $(SOURCES) $(ARGS) -lX11 -lGLX  -o screenshot-tool -w
	./screenshot-tool
	
single-headers.o:
	gcc -c source/single-headers.c $(ARGS) 