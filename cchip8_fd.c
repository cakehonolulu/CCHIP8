#include "cchip8.h"

// Fetch memory and construct the opcode based on the program counter
uint16_t m_fetch(m_chip8 *chip8)
{
	uint16_t m_opcode = (RAM[PC]) << 8
				| (RAM[PC + 1]);

	return m_opcode;
}

// Using switch cases, after we fetch the current opcode in the program counter, emulate the instruction
void m_exec(m_chip8 *chip8)
{
	M_OPCODE = m_fetch(chip8);

#ifdef DEBUG
	printf("opcode: 0x%x\n", M_OPCODE);
#endif

	switch(M_OPCODE & 0xF000)
	{
		/*
			0x0000 opcode subfamily
		*/
		case 0x0000:
			switch (M_OPCODE & 0x00FF)
			{
				/*
					00E0:
					Clear the screen
				*/
				case 0x00E0:
					// Set each display pixel to 0 (Black)
					for (int i = 0; i < (CHIP8_ROWS * CHIP8_COLUMNS); ++i)
					{
                        chip8->m_display[i] = 0;
                    }

                    // Redraw the entire screen with black pixels
                    chip8->m_redraw = true;

                    // Increment the Program Counter Register
                    PC += 2;
                    break;

                /*
					00EE:
					Return from a subroutine
				*/
				case 0x00EE:
					// TODO: Stack push-pop function to simplify the code

					// Decrease the stack pointer by 1
					SP--;

					// Set the program counter to the stack value pointed by stack pointer
					PC = SS[SP];

					// Increase the Program Counter by 2 (Returning effectively from the subroutine)
					PC += 2;
					break;

				default:
					// Break from case 0x00FF
					break;
			}

			// Break from case 0x0000
			break;

		/*
			1NNN:
			Jumps to address NNN.
		*/
		case 0x1000:
			/*
				Do the same as 2NNN but don't touch the stack, as this is a jump, not a subroutine call where
				we need to push the return address to the stack.

				Set the program counter to the address NNN found in the current opcode.
			*/
			PC = M_GET_NNN_FROM_OPCODE(M_OPCODE);

#ifdef DEBUG
			printf("Jumping to 0x%x\n", M_GET_NNN_FROM_OPCODE(M_OPCODE));
#endif

			break;

		/*
			2NNN:
			Call the subroutine located at 0xNNN
		*/
		case 0x2000:

#ifdef DEBUG
			printf("2NNN (%x) [NNN -> 0x%x]\n", M_OPCODE, M_GET_NNN_FROM_OPCODE(M_OPCODE));
			printf("2NNN -> PC: 0x%x\n", PC);
#endif
			/*
				cake (24/10/2021):
				When I first made the 2NNN implementation, I forgot that I had to push the current program counter
				address to the stack and update the stack pointer accordingly
			*/

			// TODO: Stack push-pop function to simplify the code

			// Push the current program counter to the stack at current stack pointer position
			SS[SP] = PC;

			// Increase the stack pointer by 1
			SP++;

			// Set the program counter to the address provided by the opcode
			PC = M_GET_NNN_FROM_OPCODE(M_OPCODE);

#ifdef DEBUG
			printf("2NNN Jumped to: 0x%x\n", PC);
#endif
			break;

		/*
			3XNN:
			Skips the next instruction if Vx equals to NN
		*/
		case 0x3000:
			// Check if X register contains NN bytes
			if (REGS[M_OPC_0X00(M_OPCODE)] == (M_GET_NN_FROM_OPCODE(M_OPCODE)))
				// If true, increment PC by 4
                PC += 4;
            else
            	// Else, increment PC by 2
                PC += 2;
            break;

        /*
			4XNN:
			Skips the next instruction if Vx is not equal to NN
		*/
        // Tip: It's the inverse of 3XNN
        case 0x4000:
			// Check if X register doesn't contain NN bytes
			if (REGS[M_OPC_0X00(M_OPCODE)] != (M_GET_NN_FROM_OPCODE(M_OPCODE)))
				// If true, increment PC by 4
				PC += 4;
			else
				// Else, increment PC by 2
				PC += 2;

			break;

		/*
			5XY0:
			Skip the next instruction if Vx = Vy
		*/
		// Tip: The easiest of all the 3XNN-4XNN-5XNN series, checks if Vx and Vy are the same
		case 0x5000:
			if (REGS[M_OPC_0X00(M_OPCODE)] == REGS[M_OPC_00X0(M_OPCODE)])
				// Check if X register doesn't contain NN bytes
				PC += 4;
			else
				// Else, increment PC by 2
				PC += 2;

			break;

		/*
			6XNN:
			Set VX to NN
		*/
		case 0x6000:
#ifdef DEBUG
			printf("6XNN (%x) [NN -> 0x%x]\n", M_OPCODE, M_OPCODE & 0x00FF);
#endif
			// Get NN from current opcode and store it into registers[x]
			REGS[M_OPC_0X00(M_OPCODE)] = M_GET_NN_FROM_OPCODE(M_OPCODE);
			// Increment PC by 2
			PC += 2;
			break;

		/*
			7XNN:
			Adds NN to VX. (Carry flag is not changed);
		*/
		case 0x7000:
			// Calculate Vx and add it NN (NN from Current Opcode)
			REGS[M_OPC_0X00(M_OPCODE)] += M_GET_NN_FROM_OPCODE(M_OPCODE);
			// Increment PC by 2
			PC += 2;
			break;

		/*
			0x8000 opcode subfamily
		*/
		case 0x8000:
			// Determine which opcode we're dealing with based on last digit
			switch (M_OPCODE & 0x000F)
			{
				/*
					8XY0:
					Set Vx to the value of Vy
				*/
				case 0x0000:
					// Find V(x) register and set it to V(y)'s value
                    REGS[M_OPC_0X00(M_OPCODE)] = REGS[M_OPC_00X0(M_OPCODE)];
                    // Increment PC by 2
                    PC += 2;
                    break;

                /*
                	8XY1:
					Sets VX to VX or VY. (Bitwise OR operation)
                */
                case 0x0001:
                	// Find V(x) register value, OR-it to V(y) register and save it to V(x)
					REGS[M_OPC_0X00(M_OPCODE)] |= REGS[M_OPC_00X0(M_OPCODE)];
					// Increment PC by 2
                    PC += 2;
                	break;

                /*
					8XY2:
					Sets VX to VX and VY. (Bitwise AND operation)
				*/
				case 0x0002:
					// Find V(x) register value, AND-it to V(y) register and save it to V(x)
					REGS[M_OPC_0X00(M_OPCODE)] &= REGS[M_OPC_00X0(M_OPCODE)];
					// Increment PC by 2
                    PC += 2;
					break;

				/*
					8XY3:
					Sets VX to VX xor VY.
				*/
				case 0x0003:
					// Find V(x) register value, AND-it to V(y) register and save it to V(x)
					REGS[M_OPC_0X00(M_OPCODE)] ^= REGS[M_OPC_00X0(M_OPCODE)];
					// Increment PC by 2
                    PC += 2;
					break;

				/*
					8XY4:
					Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
				*/
				case 0x0004:
					// Find V(x) register value, add V(y) value to it
					REGS[M_OPC_0X00(M_OPCODE)] += REGS[M_OPC_00X0(M_OPCODE)];

					// Sum V(x) and V(y)
					uint16_t m_add = REGS[M_OPC_0X00(M_OPCODE)] + REGS[M_OPC_00X0(M_OPCODE)];

					/*
						Check if the addition overflowed
						If it overflowed, set VF (Flag Register) to 1, else to 0
						Remember, unsigned chars are 8 bit values, that means
						that the maximum integer they can hold is 255.
						We'll use a 16-bit wide variable (unsigned short) to
						perform an addition and compare it against UCHAR_MAX
						to decide wether to flip VF or not.
					*/
                    if(m_add > UCHAR_MAX)
                        REGS[VF] = 1;
                    else
                        REGS[VF] = 0;

                    // Increment PC by 2
                    PC += 2;
					break;

				/*
					8XY5:
					VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
				*/
				case 0x0005:
					// Substract Vy from Vx
					REGS[M_OPC_0X00(M_OPCODE)] -= REGS[M_OPC_00X0(M_OPCODE)];

					/*
						Check if there's a borrow, if it exists, flip VF (Flag Register).
						It's a simple task to do, using algebra:
						if x1 > x2; (x1 - x2) > 0 there's no borrow, else, there is
					*/
					if (REGS[M_OPC_0X00(M_OPCODE)] > REGS[M_OPC_00X0(M_OPCODE)])
                        REGS[VF] = 1;
                    else
                        REGS[VF] = 0;

                    // Increment PC by 2
                    PC += 2;
                    break;

				case 0x0007:
					if(REGS[M_OPC_0X00(M_OPCODE)] > REGS[(M_OPCODE & 0x00F0) >> 4])
                        REGS[0xF] = 0;
                    else
                        REGS[0xF] = 1;

                    REGS[M_OPC_0X00(M_OPCODE)] = REGS[(M_OPCODE & 0x00F0) >> 4] - REGS[M_OPC_0X00(M_OPCODE)];
                    PC += 2;
                    break;

                default:
                	break;
			}

			break;

		case 0xA000: // [ANNN] Sets I to the address NNN
#ifdef DEBUG
			printf("ANNN (%x) [NNN -> 0x%x]\n", 
				M_GET_NNN_FROM_OPCODE(M_OPCODE), M_GET_NNN_FROM_OPCODE(M_OPCODE));
#endif
			I = M_GET_NNN_FROM_OPCODE(M_OPCODE);
			PC += 2;
			break;

		/*
			CXNN:
			Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
		*/
		case 0xC000:
			REGS[M_OPC_0X00(M_OPCODE)] = (rand() % (0xFF + 1)) & (M_OPCODE & 0x00FF);
            PC += 2;
            break;

#if defined(__MINGW32__) || defined(__MINGW64__)
		case 0xD000: ;
#endif

#ifdef __unix__
		case 0xD000:
#endif
#ifdef DEBUG
			printf("Drawing Sprite...\n");
#endif
			// X coord is the closest byte to MSB
			int m_xcoord = REGS[(M_OPCODE & 0x0F00) >> 8];

            int m_ycoord = REGS[(M_OPCODE & 0x00F0) >> 4];

            int ht = M_OPCODE & 0x000F;
            int wt = 8;
            REGS[0xF] = 0;

			for (int i = 0; i < ht; i++)
            {
                int pixel = RAM[I + i];
                for (int j = 0; j < wt; j++)
                {
                    if ((pixel & (0x80 >> j)) != 0)
                    {
                        int index = ((m_xcoord + j) + ((m_ycoord + i) * 64)) % 2048;
                        if (chip8->m_display[index] == 1)
                        {
                            REGS[0xF] = 1;
                        }
                        chip8->m_display[index] ^= 1;
                    }
                }
            }

    		chip8->m_redraw = true;
			PC += 2;
			break;

		case 0xE000:
			switch (M_OPCODE & 0x00FF)
			{
				case 0x00A1:
					if (chip8->m_keyboard[REGS[M_OPC_0X00(M_OPCODE)]] == 0)
                        PC +=  4;
                    else
                        PC += 2;
                    break;
			}
			break;

		case 0xF000:
			switch (M_OPCODE & 0x00FF)
			{

				/*
					FX07 (Inverse of FX15):
					Sets VX to the value of the delay timer.
				*/
				case 0x0007:
					REGS[M_OPC_0X00(M_OPCODE)] = chip8->m_delaytmr;
                    PC += 2;
					break;

				/*
					FX15 (Inverse of FX07):
					Sets the delay timer to VX.
				*/
				case 0x0015:
					chip8->m_delaytmr = REGS[M_OPC_0X00(M_OPCODE)];
                    PC += 2;
                    break;

                case 0x0018:
                	chip8->m_soundtmr = REGS[M_OPC_0X00(M_OPCODE)];
                    PC += 2;
                    break;

				/*
					FX29:
					Sets I to the location of the sprite for the character in VX.
					Characters 0-F (in hexadecimal) are represented by a 4x5 font.
				*/
				case 0x0029:
    					I = (REGS[M_OPC_0X00(M_OPCODE)] * 0x5);
    					PC += 2;
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
    				RAM[I] = REGS[M_OPC_0X00(M_OPCODE)] / 100;
#ifdef DEBUG
					printf("idx (%d)\n", RAM[I]); 
#endif
    				RAM[I + 1] = (REGS[M_OPC_0X00(M_OPCODE)] / 10) % 10;
#ifdef DEBUG
					printf("idx+1 (%d)\n", RAM[I + 1]);
#endif
    				RAM[I + 2] = (REGS[M_OPC_0X00(M_OPCODE)] % 100) % 10;
#ifdef DEBUG
					printf("idx+2 (%d)\n", RAM[I + 2]);
#endif
					PC += 2;
					break;
				
				/*
					FX55:
					Stores V0 to VX (including VX) in memory starting at address I.
					The offset from I is increased by 1 for each value written, but I itself is left unmodified.
				*/
				case 0x0065:
#ifdef DEBUG
					printf("m_currentopcode 0x%x, m_currentopcode & 0x%x, m_currentopcode &>> 0x%x\n", M_OPCODE, M_OPCODE & 0x0F00, M_OPC_0X00(M_OPCODE));
#endif
					/* 
						Use a for() loop to do this task, starting at V0, iterate F(x) times (Calculated above) ending
						at V(x) register. Each time we enter the for() loop, load in the value at the index register
						onto the current register pointed by m_currentregister in the loop
					*/
					for (size_t m_currentregister = 0; m_currentregister <= M_OPC_0X00(M_OPCODE); m_currentregister++)
					{
						REGS[m_currentregister] = RAM[I + m_currentregister];
					}

					// Increase the program counter by 2
					PC += 2;
				}

			break;
		
		default:
			printf("Uninmplemented opcode 0x%x\n", M_OPCODE);
			chip8->m_isUnimplemented = true;
			return;
	}
}
