LDLIBS=-lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl
CFLAGS=-Wall -std=c11 -pedantic -Werror=vla
SOURCES=main.c common/bmp_load.c common/matrix.c common/model.c common/loadShader.c
TARGET=main

all: $(SOURCES) 
	gcc $(SOURCES) $(CFLAGS) $(LDLIBS) -o $(TARGET)
	
clean:
	rm -f *.o $(TARGET)

install:
	#install xo 
uninstall:
	#rm xo

.PHONY: all clean