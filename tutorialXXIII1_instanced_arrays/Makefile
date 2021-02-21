LDLIBS=-lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl -lassimp
CFLAGS=-Wall
SOURCES=main.c matrix.c
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