LDLIBS=-lglfw3 -lGL -lGLEW -lm -lX11 -lpthread -ldl
CFLAGS=-Wall
SOURCES=main.c bmp_load.c
TARGET=tutorial43_texture_reverse_direction_shader

all: $(SOURCES) 
	gcc $(SOURCES) $(CFLAGS) $(LDLIBS) -o $(TARGET)
	
clean:
	rm -f *.o $(TARGET)

install:
	#install xo 
uninstall:
	#rm xo

.PHONY: all clean