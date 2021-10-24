# CCHIP8⃣ - A Multiplatform SDL2, CHIP8-Interpreter Emulator!

## About

C CHIP-8 (CCHIP8) is a CHIP8 Virtual Machine Interpreter Emulator written in [C](https://en.wikipedia.org/wiki/C_(programming_language)) that Fetches, Interprets and Executes CHIP8's Virtual Machine Instructions 💱.

Virtual-Machine-Interpreter-Emulator (VMIE™)? What sort of *cthulhu* 🐙 is that?

Well, to understand what that means we have to go way back in time (Concretely in the Late-70s to Early-80s) 🕹.

*Technically* speaking, CHIP8 never existed physically, it wasn't neither a console nor a computer; then, what it is❓

Well, it's a [Virtual Machine](https://en.wikipedia.org/wiki/Virtual_machine) 🖥!

*Sigh* oh well... Definitely not the Virtual Machines you're used to use, but more of a Programming Language that could be interpreted by different machines and archieve the same result in all of them! ☑

Does that explanation *turn on a 💡 on you*?

There's a programming language that works *exactly* like that (Well, *not exactly* but generally speaking...) 🧠

Yes, you're thinking right... **Python** 🐍!

The meaning **virtual** makes sense because all the programs written in CHIP8 would be able to run in all systems were a CHIP8 Virtual Machine Interpreter was found (Same as Python❗)

CCHIP8 aims to be *the interpreter* that translates CHIP8s Instructions to your computer, in a easy, fast and pretty experience. 💫

## Compiling

### For Linux (Using GCC 11):
```sh
gcc cchip8.c cchip8_fd.c -Wall -Wextra -lSDL2 -o cchip8
```

### Cross-compiling to Windows (Using GCC 10 and targetting x86_64)
```sh
x86_64-w64-mingw32-gcc-10-win32 cchip8.c cchip8_fd.c -I$Win32SDL2Headers -L$Win32SDL2Libs -lmingw32 -lSDL2main -lSDL2 -o cchip8.exe
```

Don't forget to put MINGW SDL2 Development Kit in your path! Personally I point $Win32SDL2Headers to where MINGW SDL2 Headers are and $Win32SDL2Libs where SDL2 Libs are


(Add -DDEBUG switch if you want to print debug output on the program's terminal)

## Running
### Under Linux

```./cchip8 [arguments] [programname]```

Arguments:

-d Enable the in-built debugger

-D Same as -d

### Under Windows

Simply open cchip8.exe and it'll load any program you put inside the same directory with this name 'rom.ch8'

## Motivation
Understand how SDL2 works as I had never used it before

Understand how LLE Emulation (Hardware level) works.

CHIP8 is a small-enough interpreter that can be easily tackled using LLE and a simple fetch & decode cycle where using switch cases won't really affect performance that much considering nowaday's regular PC computing power

## TODOs

Maybe switch from a switch() type opcode interpretation to a function pointer-based one
