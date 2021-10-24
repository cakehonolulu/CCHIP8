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

	if (m_opcode == 0xe0)
	{
		for (int i = 0; i < 2048; ++i)
					{
                        chip8->m_display[i] = 0;
                    }
                    chip8->m_redraw = true;
                    chip8->m_programcounter += 2;
	}

	switch(m_opcode & 0xF000)
	{
		case 0x0000:
			switch (m_opcode & 0x00FF)
			{
				case 0x00EE:
					// TODO: Stack push-pop function to simplify the code
					chip8->m_stackp--;
					chip8->m_programcounter = chip8->m_stack[chip8->m_stackp];
					chip8->m_programcounter += 2;
					break;
			}
			break;

		/*
			1NNN:
			Jumps to address NNN.
		*/
		case 0x1000:
			/*
				Do the same as 2NNN but don't touch the stack, as this is a jump, not a subroutine call where
				we need to push the return address to the stack
			*/
			chip8->m_programcounter = chip8->m_currentopcode & 0x0FFF;
#ifdef DEBUG
			printf("Jumping to 0x%x\n", chip8->m_currentopcode & 0x0FFF);
#endif
			break;

		case 0x2000: // [2NNN] Cals subroutine at NNN
#ifdef DEBUG
			printf("2NNN (%x) [NNN -> 0x%x]\n", chip8->m_currentopcode, chip8->m_currentopcode & 0x0FFF);
			printf("2NNN -> PC: 0x%x\n", chip8->m_programcounter);
#endif
			/*
				When I first made the 2NNN implementation, I forgot that I had to push the current program counter
				ddress to the stack and update the stack pointer accordingly
			*/

			// TODO: Stack push-pop function to simplify the code
			chip8->m_stack[chip8->m_stackp] = chip8->m_programcounter;
			chip8->m_stackp++;

			chip8->m_programcounter = chip8->m_currentopcode & 0x0FFF;

#ifdef DEBUG
			printf("2NNN Jumped to: 0x%x\n", chip8->m_programcounter);
#endif
			break;

		/*
			3XNN:
			Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block);
		*/
		case 0x3000:
			if (chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] == (chip8->m_currentopcode & 0x00FF))
                chip8->m_programcounter += 4;
            else
                chip8->m_programcounter += 2;

            break;

        case 0x4000:
			if (chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] != (chip8->m_currentopcode & 0x00FF))
				chip8->m_programcounter += 4;
			else
				chip8->m_programcounter += 2;

			break;

		case 0x6000: // [6XNN] Sets Vx to NN
#ifdef DEBUG
			printf("6XNN (%x) [NN -> 0x%x]\n", chip8->m_currentopcode, chip8->m_currentopcode & 0x00FF);
#endif
			chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] =
				chip8->m_currentopcode & 0x00FF;
			chip8->m_programcounter += 2;
			break;

		/*
			7XNN:
			Adds NN to VX. (Carry flag is not changed);
		*/
		case 0x7000:
			chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] += chip8->m_currentopcode & 0x00FF;
			chip8->m_programcounter += 2;
			break;

		case 0x8000:
			switch (m_opcode & 0x000F)
			{
				case 0x0000:
                    chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] = chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4];
                    chip8->m_programcounter += 2;
                    break;

				case 0x0002:
					chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] &= chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4];
                    chip8->m_programcounter += 2;
					break;

				case 0x0004:
					chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] += chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4];
                    if(chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4] > (0xFF - chip8->m_registers[(chip8->m_currentopcode & 0x0F00) >> 8]))
                        chip8->m_registers[0xF] = 1;
                    else
                        chip8->m_registers[0xF] = 0;
                    chip8->m_programcounter += 2;
					break;

				case 0x0005:
					if(chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4] > chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)])
                        chip8->m_registers[0xF] = 0;
                    else
                        chip8->m_registers[0xF] = 1;

                    chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] -= chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4];
                    chip8->m_programcounter += 2;
                    break;

				case 0x0007:
					if(chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] > chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4])
                        chip8->m_registers[0xF] = 0;
                    else
                        chip8->m_registers[0xF] = 1;

                    chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] = chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4] - chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)];
                    chip8->m_programcounter += 2;
                    break;
			}

			break;

		case 0xA000: // [ANNN] Sets I to the address NNN
#ifdef DEBUG
			printf("ANNN (%x) [NNN -> 0x%x]\n", 
				chip8->m_currentopcode & 0x0FFF, chip8->m_currentopcode & 0x0FFF);
#endif
			chip8->m_index = chip8->m_currentopcode & 0x0FFF;
			chip8->m_programcounter += 2;
			break;

		/*
			CXNN:
			Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		*/
		case 0xC000:
			chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] = (rand() % (0xFF + 1)) & (chip8->m_currentopcode & 0x00FF);
            chip8->m_programcounter += 2;
            break;

#ifdef __MINGW32__ || __MINGW64__
		case 0xD000: ;
#endif

#ifdef __unix__
		case 0xD000:
#endif
#ifdef DEBUG
			printf("Drawing Sprite...\n");
#endif
			// X coord is the closest byte to MSB
			int m_xcoord = chip8->m_registers[(chip8->m_currentopcode & 0x0F00) >> 8];

            int m_ycoord = chip8->m_registers[(chip8->m_currentopcode & 0x00F0) >> 4];

            int ht = chip8->m_currentopcode & 0x000F;
            int wt = 8;
            chip8->m_registers[0xF] = 0;

			for (int i = 0; i < ht; i++)
            {
                int pixel = chip8->m_memory[chip8->m_index + i];
                for (int j = 0; j < wt; j++)
                {
                    if ((pixel & (0x80 >> j)) != 0)
                    {
                        int index = ((m_xcoord + j) + ((m_ycoord + i) * 64)) % 2048;
                        if (chip8->m_display[index] == 1)
                        {
                            chip8->m_registers[0xF] = 1;
                        }
                        chip8->m_display[index] ^= 1;
                    }
                }
            }

    		chip8->m_redraw = true;
			chip8->m_programcounter += 2;
			break;

		case 0xE000:
			switch (m_opcode & 0x00FF)
			{
				case 0x00A1:
					if (chip8->m_keyboard[chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)]] == 0)
                        chip8->m_programcounter +=  4;
                    else
                        chip8->m_programcounter += 2;
                    break;
			}
			break;

		case 0xF000:
			switch (m_opcode & 0x00FF)
			{

				/*
					FX07 (Inverse of FX15):
					Sets VX to the value of the delay timer.
				*/
				case 0x0007:
					chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] = chip8->m_delaytmr;
                    chip8->m_programcounter += 2;
					break;

				/*
					FX15 (Inverse of FX07):
					Sets the delay timer to VX.
				*/
				case 0x0015:
					chip8->m_delaytmr = chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)];
                    chip8->m_programcounter += 2;
                    break;

                case 0x0018:
                	chip8->m_soundtmr = chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)];
                    chip8->m_programcounter += 2;
                    break;

				/*
					FX29:
					Sets I to the location of the sprite for the character in VX.
					Characters 0-F (in hexadecimal) are represented by a 4x5 font.
				*/
				case 0x0029:
    					chip8->m_index = (chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] * 0x5);
    					chip8->m_programcounter += 2;
    					break;
				/*
					FX33:
					Stores the binary-coded decimal representation of VX, with the most significant
					of three digits at the address in I, the middle digit at I plus 1, and the
					least significant digit at I plus 2.
					(In other words, take the decimal representation of VX, place the hundreds digit
					in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.);
				*/
				case 0x0033:
#ifdef DEBUG
					printf("FX33 Opcode!\n"); 
#endif
    				chip8->m_memory[chip8->m_index] = chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] / 100;
#ifdef DEBUG
					printf("idx (%d)\n", chip8->m_memory[chip8->m_index]); 
#endif
    				chip8->m_memory[chip8->m_index + 1] = (chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] / 10) % 10;
#ifdef DEBUG
					printf("idx+1 (%d)\n", chip8->m_memory[chip8->m_index + 1]);
#endif
    				chip8->m_memory[chip8->m_index + 2] = (chip8->m_registers[M_OPC_0X00(chip8->m_currentopcode)] % 100) % 10;
#ifdef DEBUG
					printf("idx+2 (%d)\n", chip8->m_memory[chip8->m_index + 2]);
#endif
					chip8->m_programcounter += 2;
					break;
				
				/*
					FX55:
					Stores V0 to VX (including VX) in memory starting at address I.
					The offset from I is increased by 1 for each value written, but I itself is left unmodified.
				*/
				case 0x0065:
#ifdef DEBUG
					printf("m_currentopcode 0x%x, m_currentopcode & 0x%x, m_currentopcode &>> 0x%x\n", chip8->m_currentopcode, chip8->m_currentopcode & 0x0F00, M_OPC_0X00(chip8->m_currentopcode));
#endif
					/* 
						Use a for() loop to do this task, starting at V0, iterate F(x) times (Calculated above) ending
						at V(x) register. Each time we enter the for() loop, load in the value at the index register
						onto the current register pointed by m_currentregister in the loop
					*/
					for (size_t m_currentregister = 0; m_currentregister <= M_OPC_0X00(chip8->m_currentopcode); m_currentregister++)
					{
						chip8->m_registers[m_currentregister] = chip8->m_memory[chip8->m_index + m_currentregister];
					}

					// Increase the program counter by 2
					chip8->m_programcounter += 2;
				}

			break;
		
		default:
			printf("Uninmplemented opcode 0x%x\n", m_opcode);
			chip8->m_isUnimplemented = true;
			return;
	}
}
