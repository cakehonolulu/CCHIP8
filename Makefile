# Setup the compiler (In this case GNU GCC)
CC = gcc

# Setup the basic compilation flags
# Warn all, extra and compile for c2x
CFLAGS = -Wall -Wextra -std=c2x

SDLFLAGS = `sdl2-config --cflags --libs` `pkg-config SDL2_ttf --cflags --libs`

LDFLAGS = -lm -lSDL2

BINARY := cchip8

all: $(BINARY)

$(BINARY): *.c
	@echo "🚧 Building..."
	$(CC) $(CFLAGS) $(SDLFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	@echo "🧹 Cleaning..."
	-@rm $(BINARY)