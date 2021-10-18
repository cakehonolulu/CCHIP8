#include <stdio.h>
#include "cchip8.h"

#define OK 0
#define FAIL 1

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
	} else {
		printf("Program file loaded successfully\n");
	}

	// Declare the CHIP8 Interpreter skeleton
	m_chip8 chip8;

	// Clear registers
	memset(&chip8.m_registers, 0, sizeof(chip8.m_registers));

}