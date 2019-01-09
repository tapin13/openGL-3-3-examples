LDLIBS=-lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl
CFLAGS=-Wall
SOURCES=main.c
TARGET=tutorial34_shaders_more_attributes

all: $(SOURCES) 
	gcc $(SOURCES) $(CFLAGS) $(LDLIBS) -o $(TARGET)
	
clean:
	rm -f *.o $(TARGET)

install:
	#install xo 
uninstall:
	#rm xo

.PHONY: all clean