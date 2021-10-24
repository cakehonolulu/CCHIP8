#include "cchip8.h"

#define OK 0
#define FAIL 1

#define CHIP8_INITIAL_PC 0x200

#ifdef __MINGW32__ || __MINGW64__
int WinMain(int argc, char **argv)
#endif

#ifdef __unix__
int main(int argc, char **argv)
#endif
{
	printf("CCHIP8 - A C-21 Multiplatform CHIP8 Interpreter Emulator by cakehonolulu\n");

#ifdef __unix__
	printf("Running under Linux!\n");
	// Check for commandline arguments
	if (argc < 2)
	{
		printf("Usage: ./cchip8 [flags] [progname]\n");
		printf("Command-line switches:\n");
		printf("-[d or D] Enable the built-in debugger\n");
		return FAIL;
	}
#endif

	// Implement a debug mode program flag (Enabled through command line arguments)
	bool m_dbgmode = false;

#ifdef __MINGW32__ || __MINGW64__
	printf("Running under Windows!\n");
	// Use a char array to store the program name for later use
	const char *m_filename = "rom.ch8";
#endif

#ifdef __unix__
	const char *m_filename;

	// Simple for loop that checks for command-line switches and stores the program name from argv to *m_filename
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (argv[i][1] == 'd' || argv[i][1] == 'D')
			{
				// If "-d" or "-D" switches are found, enable debugger mode 
				m_dbgmode = true;
				printf("Debugger enabled!\n");
			} else {
				// If the switch doesn't exist, warn the user and exit
				printf("Invalid switch!\n");
				return FAIL;
			}
		} else {
			// Store the filename into m_filename
			m_filename = argv[i];

			// Print the filename
			printf("Loading %s...\n", m_filename);
		}
	}
#endif
	// Use the FILE directive to access a file
	FILE *m_prg;

	// Open the file in binary mode (And read-only)
	m_prg = fopen(m_filename, "rb");

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

	// Set stack pointer to 0
	chip8.m_stackp = 0;

	// Initialize the memory
	memset(&chip8.m_memory, 0, sizeof(chip8.m_memory));

	// Initialize the keyboard data
	memset(&chip8.m_keyboard, 0, sizeof(chip8.m_keyboard));

	// Initialize the displau
	memset(&chip8.m_display, 0, sizeof(chip8.m_display));

	// Initialize internal pixel display
	memset(&chip8.m_pixels, 0, sizeof(chip8.m_pixels));

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
		chip8.m_memory[CHIP8_INITIAL_PC + i] = m_prg_buf[i];

#ifdef DEBUG
		printf("0x%x ", chip8.m_memory[CHIP8_INITIAL_PC + i]);

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

	// Declare both the window and Surface to use SDL2 abilities
	SDL_Window   *m_window;
	SDL_Renderer  *m_renderer;
	SDL_Texture *m_texture;

	// Init SDL2
	SDL_Init(SDL_INIT_EVERYTHING);

	// Create a 500 x 500 (px) window
	m_window = SDL_CreateWindow("CCHIP8 Emulator - cakehonolulu (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320,
                              SDL_WINDOW_SHOWN);

	// Set screen as a pointer to the window's surface
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	// Adjust the renderer size
	SDL_RenderSetLogicalSize(m_renderer, 640, 320);
	
	// Setup the texture trick that'll enable us to display emulator output
	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

	// Create an SDL2 event
	SDL_Event event;

	// Update the framebuffer with all the changes
	SDL_UpdateWindowSurface(m_window);

	// If Debug Mode is enabled, instead of jumping into the regular emulation, open the debugger instead
	if (m_dbgmode == true)
	{
		printf("Entered Debug Mode!\n");
	}

	while (true)
	{
		// Use a while() block waiting for SDL_PollEvent to intercept keyboard and sound events
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					SDL_DestroyWindow(m_window);
					SDL_Quit();
					exit(9);
					break;

				case SDL_KEYDOWN:
					m_exec(&chip8);

					printf("\n\nCurrent OP: 0x%X\n", chip8.m_currentopcode);

					for (int i = 0; i < 16; i++)
					{
				    	printf("V Reg %X: 0x%X\n",i , chip8.m_registers[i]);
					}

					printf("Index Reg: 0x%X\n", chip8.m_index);
   					printf("PC Reg: 0x%X\n", chip8.m_programcounter);
   					printf("SP Reg: 0x%X\n", chip8.m_stackp);
   					printf("Delay Timer Reg: 0x%X\n", chip8.m_delaytmr);
   					printf("Sound Timer Reg: 0x%X\n", chip8.m_soundtmr);
   					break;

				default:
					break;
			}	
		}

			/*
				Check if opcode is unimplemented, if true, exit the main emulator loop (Fetch & Decode),
				clear the SDL2 surface, close the GUI window and quit SDL2 altogether.
			*/
			if (chip8.m_isUnimplemented == true)
			{
				printf("Exiting the main loop...\n");
				SDL_DestroyWindow(m_window);
				SDL_Quit();
				return FAIL;
			} else {
				// Execute the fetch & decode
				m_exec(&chip8);
			}

			if (chip8.m_redraw)
			{
				chip8.m_redraw = false;
				
				uint32_t pixels[32 * 64];

           		for (int i = 0; i < 32 * 64; i++)
           		{
               		if (chip8.m_display[i] == 0)
               		{
						pixels[i] = 0xFF000000;
	           		} else {
						pixels[i] = 0xFFFFFFFF;
					}
				}

				SDL_UpdateTexture(m_texture, NULL, pixels, 64 * sizeof(uint32_t));
				SDL_RenderClear(m_renderer);
				SDL_RenderCopy(m_renderer, m_texture, NULL, NULL);
				SDL_RenderPresent(m_renderer);
			}

			// Update timers
			if (chip8.m_delaytmr > 0)
			{
				chip8.m_delaytmr--;
			}

			if (chip8.m_soundtmr > 0)
			{
				if(chip8.m_soundtmr == 1)
				{
					// Sound playing routine
				}
						
				chip8.m_soundtmr--;
			}

			usleep(1500);
		}
	}

