# Setup the Linux Compiler (In this case GNU GCC)
CC = gcc

# Setup the Windows Compiler (In this cross-compiling using mingw64)
MINGW64 = x86_64-w64-mingw32-gcc-10-win32

# Setup the basic compilation flags
# Warn all, extra and compile for c2x
CFLAGS = -Wall -Wextra -std=c2x

SDLFLAGS = `sdl2-config --cflags --libs` `pkg-config SDL2_ttf --cflags --libs`

LDFLAGS = -lm -lSDL2

ifdef WIN32
BINARY := cchip8.exe
endif

ifdef UNIX
BINARY := cchip8
endif

all: $(BINARY)

ifdef WIN32
$(BINARY): *.c
	@echo "🚧 Building..."
	$(MINGW64) -I$(Win32SDL2Headers) -L$(Win32SDL2Libs) $^ -o $@ -lmingw32 -lSDL2main -lSDL2 -lm
endif

ifdef UNIX
$(BINARY): *.c
	@echo "🚧 Building..."
	$(CC) $(CFLAGS) $(SDLFLAGS) $^ -o $@ $(LDFLAGS)
endif

clean:
	@echo "🧹 Cleaning..."
	-@rm $(BINARY)