#include <stdbool.h>

typedef struct chip8
{
	// CHIP8 - Arithmetic Registers
	unsigned char m_registers[16];

	// CHIP8 - Index Register
	unsigned char m_index;

	// CHIP8 - Program Counter (PC) Register
	unsigned char m_programcounter;

	// CHIP8 - Stack Pointer (SP) Register
	unsigned char m_stack[16];

	// CHIP8 - Memory
	// CHIP8 has a total of 4 KiloBytes (4096 bytes) of
	// system memory. You can find a CHIP8 memory 
	// map at http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0
	// Declare an array containing (4096) entries
	unsigned char m_memory[4096];

	// CHIP8 - Input
	// CHIP8 has 16 total keyboard keys
	// Declare an array containing 16 entries
	// to manage keyboard inputs
	unsigned char m_keyboard[16];

	// CHIP8 - Output
	// CHIP8 has a (row:col) 64 by 32 pixel buffer
	// Declare an array containing (64*32=2048) entries
	// to address the totality of the video buffer
	unsigned char m_display[64 * 32];

	// CHIP8 - Timer Registers
	unsigned char m_soundtmr;
	unsigned char m_delaytmr;

	// This bool will be checked against in the main emulator loop
	// in order to decide wether to draw or not the screen
	bool m_redraw;
} m_chip8;