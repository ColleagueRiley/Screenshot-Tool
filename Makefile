SOURCES = source/main.c single-headers.o
ARGS = -lm -I./include -I./include/deps -s
OPARGS = -fpredictive-commoning -O3

all:
	make single-headers.o
	gcc $(SOURCES) $(ARGS) $(OPARGS) -lX11 -lGLX -lGL -o screenshot-tool
	strip ./screenshot-tool

debug:
	make single-headers.o
	gcc $(SOURCES) $(ARGS) -lX11 -lGL -lGLX -o screenshot-tool -g 
	./screenshot-tool

install:
	sudo mv screenshot-tool /usr/bin
	sudo mkdir -p /usr/share/icons/r-screenshot-tool
	sudo mv logo.png /usr/share/icons/r-screenshot-tool
	
single-headers.o:
	gcc -c source/single-headers.c $(ARGS)