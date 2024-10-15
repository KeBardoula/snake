CC = gcc
CFLAGS = -Wall -Wextra -std=c99 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_ttf

OBJ = main.o snake.o game.o graphics.o

snake: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o snake