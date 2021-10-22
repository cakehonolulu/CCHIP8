#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

#define CHIP8_FONT_SIZE 80

#define CHIP8_COLUMNS 64
#define CHIP8_ROWS 30

// Default CHIP8 font
static const unsigned char m_font[CHIP8_FONT_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct chip8
{
	// CHIP8 - Arithmetic Registers
	uint16_t m_registers[16];

	// CHIP8 - Index Register
	uint16_t m_index;

	// CHIP8 - Program Counter (PC) Register
	uint16_t m_programcounter;

	// CHIP8 - Stack Pointer (SP) Register
	uint16_t m_stack[16];
	uint16_t m_stackp;

	// CHIP8 - Memory
	// CHIP8 has a total of 4 KiloBytes (4096 bytes) of
	// system memory. You can find a CHIP8 memory 
	// map at http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#0.0
	// Declare an array containing (4096) entries
	uint8_t m_memory[4096];

	// CHIP8 - Input
	// CHIP8 has 16 total keyboard keys
	// Declare an array containing 16 entries
	// to manage keyboard inputs
	uint8_t m_keyboard[16];

	// CHIP8 - Output
	// CHIP8 has a (row:col) 64 by 32 pixel buffer
	// Declare an array containing (64*32=2048) entries
	// to address the totality of the video buffer
	uint8_t m_display[64 * 32];

	// Pixel representation for SDL texture
	uint32_t m_pixels[2048];

	// CHIP8 - Timer Registers
	uint8_t m_soundtmr;
	uint8_t m_delaytmr;

	// This bool will be checked against in the main emulator loop
	// in order to decide wether to draw or not the screen
	bool m_redraw;

	// Bool that stores the machine state
	bool m_isRunning;

	// Bool that checks if we've found an unimplemented opcode
	bool m_isUnimplemented;

	// Store current opcode
	uint16_t m_currentopcode;
} m_chip8;

void m_exec(m_chip8 *chip8);