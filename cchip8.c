#include <stdio.h>
#include "cchip8.h"

#define OK 0
#define FAIL 1

int main(int argc, char **argv)
{
	printf("CCHIP8 - A C-21 CHIP8 Interpreter Emulator for Linux by cakehonolulu\n");
	
	if (argc < 2)
	{
		printf("Usage: ./cchip8 [progname]\n");
		return FAIL;
	}

	FILE *m_prg;

	m_prg = fopen(argv[1], "rb");

	if(m_prg == NULL)
	{
		printf("Could not open the program file, exiting...\n");
	} else {
		printf("Program file loaded successfully\n");
	}

}