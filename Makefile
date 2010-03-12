CC = gcc
CFLAGS = -Wall -O3 -ffast-math -std=c99
PROG = ronde

SRCS = main.c game.c itoa.c Vector.c perlin.c

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut -lGLEW
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

test: all
	./ronde

clean:
	rm $(PROG)
	rm *~
