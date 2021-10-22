#include "cchip8.h"

// Fetch memory and construct the opcode based on the program counter
uint16_t m_fetch(m_chip8 *chip8)
{
	uint16_t m_opcode = (chip8->m_memory[chip8->m_programcounter]) << 8
				| (chip8->m_memory[chip8->m_programcounter + 1]);

	return m_opcode;
}

// Using switch cases, after we fetch the current opcode in the program counter, emulate the instruction
void m_exec(m_chip8 *chip8)
{
	uint16_t m_opcode = m_fetch(chip8);
	chip8->m_currentopcode = m_opcode;

#ifdef DEBUG
	printf("opcode: 0x%x\n", m_opcode);
#endif

	switch(m_opcode & 0xF000)
	{
		case 0x2000: // [2NNN] Cals subroutine at NNN
#ifdef DEBUG
			printf("2NNN (%x) [NNN -> 0x%x]\n", chip8->m_currentopcode, chip8->m_currentopcode & 0x0FFF);
			printf("2NNN -> PC: 0x%x\n", chip8->m_programcounter);
#endif
			chip8->m_programcounter = chip8->m_currentopcode & 0x0FFF;

#ifdef DEBUG
			printf("2NNN Jumped to: 0x%x\n", chip8->m_programcounter);
#endif
			break;

		case 0x6000: // [6XNN] Sets Vx to NN
#ifdef DEBUG
			printf("6XNN (%x) [NN -> 0x%x]\n", chip8->m_currentopcode, chip8->m_currentopcode & 0x00FF);
#endif
			chip8->m_registers[((chip8->m_currentopcode & 0x0F00) >> 8)] =
				chip8->m_currentopcode & 0x00FF;
			chip8->m_programcounter += 2;
			break;

		case 0xA000: // [ANNN] Sets I to the address NNN
#ifdef DEBUG
			printf("ANNN (%x) [NNN -> 0x%x]\n", 
				chip8->m_currentopcode & 0x0FFF, chip8->m_currentopcode & 0x0FFF);
#endif
			chip8->m_index = chip8->m_currentopcode & 0x0FFF;
			chip8->m_programcounter += 2;
			break;

		case 0xD000:
#ifdef DEBUG
			printf("Sprite draw placeholder\n");
#endif
			unsigned short x = chip8->m_registers[(chip8->m_index & 0x0F00) >> 8];
            unsigned short y = chip8->m_registers[(chip8->m_index & 0x00F0) >> 4];
            unsigned short height = chip8->m_index & 0x000F;
            unsigned short pixel;

            chip8->m_registers[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = chip8->m_memory[chip8->m_index + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(chip8->m_display[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            chip8->m_registers[0xF] = 1;
                        }
                        chip8->m_display[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

    		chip8->m_redraw = true;
			chip8->m_programcounter += 2;
			break;

		default:
			printf("Uninmplemented opcode 0x%x\n", m_opcode);
			chip8->m_isUnimplemented = true;
			return;
	}
}
