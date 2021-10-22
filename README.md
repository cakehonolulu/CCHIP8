# CCHIP8
A SDL2-driven, C21 CHIP8-interpreter Emulator!

## Compiling
```gcc cchip8.c cchip8_fd.c -Wall -Wextra -lSDL2```

(Add -DDEBUG switch if you want to show debug output)

## Running
```./a.out [arguments] [programname]```

Arguments:

-d Enable the in-built debugger

-D Same as -d

## Motivation
Understand how SDL2 works as I had never used it before

Understand how LLE Emulation (Hardware level) works.

CHIP8 is a small-enough interpreter that can be easily tackled using LLE and a simple fetch & decode cycle where using switch cases won't really affect performance that much considering nowaday's regular PC computing power


