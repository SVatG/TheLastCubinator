CC = gcc
CFLAGS = -Wall -O3 -ffast-math -std=c99 -ggdb
PROG = ronde

SRCS = network/main_client.c network/game.c network/game_client.c Vector.c
OLDSRCS = main.c game.c Vector.c

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut -lGLEW
endif

all: $(PROG)

old: $(OLDSRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(OLDSRCS) $(LIBS)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

test: all
	./ronde

clean:
	rm $(PROG)
	rm *~
