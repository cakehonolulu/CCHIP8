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

	// Initialize the displau
	memset(&chip8.m_display, 0, sizeof(chip8.m_display));

	// Initialize the sound and delay timers
	chip8.m_soundtmr = 0;
	chip8.m_delaytmr = 0;

#ifdef DEBUG
	printf("Initialized the emulated interpreter succesfully\n");
#endif

	// Get file size in bytes
	fseek(m_prg, 0, SEEK_END);
	size_t m_prgsz = ftell(m_prg);
	fseek(m_prg, 0, SEEK_SET);

	// Allocate a buffer for the program
	unsigned char *m_prg_buf;
	m_prg_buf = (unsigned char*) malloc(sizeof(unsigned char) * m_prgsz);

	// Error out on memory exhaustion
	if (m_prg_buf == NULL)
	{
		printf("Couldn't allocate memory, exiting...\n");
		return FAIL;
	}

	// Load the file into host memory
	fread(m_prg_buf, sizeof(unsigned char), m_prgsz, m_prg); 

#ifdef DEBUG
	printf("Program size: %d bytes\n", (unsigned int) m_prgsz);
	printf("Program Memory Dump: \n");
#endif

	// Load the program from host memory into interpreter's memory
	for (unsigned int i = 0; i < (unsigned int) m_prgsz; i++)
	{
		chip8.m_memory[(unsigned int) CHIP8_INITIAL_PC + i] = m_prg_buf[i];

#ifdef DEBUG
		printf("0x%x ", (unsigned int) chip8.m_memory[CHIP8_INITIAL_PC + i]);

		if (i == (((unsigned int) m_prgsz) - 1))
		{
			printf("\n");
		}
#endif
	}

	// Free the buffer
	free(m_prg_buf);

	// Close the file pipe
	fclose(m_prg);

	// Now load the font into the interpreter's memory
#ifdef DEBUG
	printf("Loading the font into memory...\n");
	printf("Font Memory Map: \n");
#endif

	for (unsigned int i = 0; i < CHIP8_FONT_SIZE; i++)
	{
		chip8.m_memory[i] = m_font[i];

#ifdef DEBUG
		printf("0x%x ", (unsigned int) chip8.m_memory[i]);
		
		if (i == (((unsigned int) CHIP8_FONT_SIZE) - 1))
		{
			printf("\n");
		}
#endif
	}

	chip8.m_isUnimplemented = false;

	while (true)
	{
		if (chip8.m_isUnimplemented == true)
		{
			printf("Exiting the main loop...\n");
			return FAIL;
		} else {
			// Execute the instruction
			m_exec(&chip8);
		}
	}
}

uint16_t m_fetch(m_chip8 *chip8)
{
	uint16_t m_opcode = (chip8->m_memory[chip8->m_programcounter]) << 8
    			| (chip8->m_memory[chip8->m_programcounter + 1]);

    return m_opcode;
}

void m_exec(m_chip8 *chip8)
{
	uint16_t m_opcode = m_fetch(chip8);
	chip8->m_currentopcode = m_opcode;

#ifdef DEBUG
	printf("opcode: 0x%x\n", m_opcode);
#endif

    switch(m_opcode & 0xF000)
    {
    	case ANNN: // [ANN] I = NNN (Sets I to the address NNN.)
#ifdef DEBUG
    		printf("A%x -> ANNN Instr. where NNN = addr. 0x%x\n", 
    			chip8->m_currentopcode & 0x0FFF, chip8->m_currentopcode & 0x0FFF);
#endif
    		chip8->m_index = chip8->m_currentopcode & 0x0FFF;
    		chip8->m_programcounter += 2;
    		break;

    	default:
    		printf("Uninmplemented opcode 0x%x\n", m_opcode);
    		chip8->m_isUnimplemented = true;
    		return;
    }
}