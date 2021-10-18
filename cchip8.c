#include <stdio.h>
#include "cchip8.h"

#define OK 0
#define FAIL 1

#define CHIP8_INITIAL_PC 0x200

int main(int argc, char **argv)
{
	printf("CCHIP8 - A C-21 CHIP8 Interpreter Emulator for Linux by cakehonolulu\n");
	
	// Check for commandline arguments
	if (argc < 2)
	{
		printf("Usage: ./cchip8 [progname]\n");
		return FAIL;
	}

	// Use the FILE directive to access a file
	FILE *m_prg;

	// Open the file in binary mode (And read-only)
	m_prg = fopen(argv[1], "rb");

	// Check if the file has been opened
	if(m_prg == NULL)
	{
		printf("Could not open the program file, exiting...\n");
		return FAIL;
	} else {
		printf("Program file loaded successfully\n");
	}

	// Declare the CHIP8 Interpreter skeleton
	m_chip8 chip8;

	// Initialize the registers
	memset(&chip8.m_registers, 0, sizeof(chip8.m_registers));

	// Set the index register to 0
	chip8.m_index = 0;

	// Set the program counter to 0x200
	chip8.m_programcounter = CHIP8_INITIAL_PC;

	// Initialize the stack
	memset(&chip8.m_stack, 0, sizeof(chip8.m_stack));

	// Initialize the memory
	memset(&chip8.m_memory, 0, sizeof(chip8.m_memory));

	// Initialize the keyboard data
	memset(&chip8.m_keyboard, 0, sizeof(chip8.m_keyboard));

}