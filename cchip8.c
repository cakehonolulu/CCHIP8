#include "cchip8.h"

#if defined(__MINGW32__) || defined(__MINGW64__)
/*
	NOTE:
	A good idea would probably be define main() as WinMain (Following <windows.h> spec).
	I've yet to find how to do it without a partial re-structuring of the emulator so
	it'll get prosponed.
*/
int main(int argc, char **argv)
#endif

#ifdef __unix__
int main(int argc, char **argv)
#endif
{
	printf("CCHIP8 - A C-21 Multiplatform CHIP8 Bytecode Interpreter Emulator\n");

#ifdef __unix__
	// Check for commandline arguments
	if (argc < 2)
	{
		printf("Usage: ./cchip8 [flags] [progname]\n");
		printf("Command-line switches:\n");
		printf("-[d or D] Enable the built-in debugger\n");
		printf("-no-exit  Prevents sudden emulator closes (For example, on Unimplemented Opcode)\n");
		return EXIT_FAILURE;
	}
#endif

	// Implement a debug mode program flag (Enabled through command line arguments)
	bool m_dbgmode = false;

	// Implement a no-exit switch that deals with SDL closing routine
	bool m_no_exit = false;

	// Declare a char pointer with the name of the filename to load
	const char *m_filename = NULL;

#ifdef __unix__
	bool m_foundrom = false;

	for (int i = 1; i < argc; i++)
	{
		if ((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "-D") == 0))
		{
			m_dbgmode = true;
		} else if (strcmp(argv[i], "-no-exit") == 0)
		{
			m_no_exit = true;
		} else if (m_foundrom != true)
		{
			if ((strstr(argv[i], ".ch8") != NULL) || (strstr(argv[i], ".rom") != NULL))
			{
				m_filename = argv[i];
				m_foundrom = true;
				
			}
		} else {
			if (m_foundrom == true)
			{
				printf("No more than 1 ROM file is allowed, exiting...\n");
				exit(EXIT_FAILURE);
			} else {
				printf("Unknown argument: %s\n", argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}

	if (m_filename == NULL)
	{
		printf("No ROM name specified!\n");
		exit(EXIT_FAILURE);
	}

	printf("Running under Linux!\n");

	if (m_dbgmode == true)
	{
		printf("Debug Mode Enabled!\n");
	}
	
	if (m_no_exit == true)
	{
		printf("CCHIP8 won't exit in case of Unimplemented Opcode!\n");
	}
#endif
	
#if defined(__MINGW32__) || defined(__MINGW64__)
	// Use a char array to store the program name for later use
	m_filename = "rom.ch8";
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
	printf("Running under Windows!\n");
#endif

	printf("Loading %s...\n", m_filename);

	// Use the FILE directive to access a file
	FILE *m_prg;

	// Open the file in binary mode (And read-only)
	m_prg = fopen(m_filename, "rb");

	// Check if the file has been opened
	if(m_prg == NULL)
	{
		printf("Could not open the program file, exiting...\n");
		return EXIT_FAILURE;
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
		return EXIT_FAILURE;
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

#ifdef DEBUG
	printf("Loading the font into memory...\n");
	printf("Font Memory Map: \n");
#endif

	// Now load the font into the interpreter's memory
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

	// Set the opcode unimplemented flag to false
	chip8.m_isUnimplemented = false;

	// Declare both the window and Surface to use SDL2 abilities
	SDL_Window   *m_window;
	SDL_Renderer  *m_renderer;
	SDL_Texture *m_texture;

	// Init SDL2
	// SDL_INIT_VIDEO automatically enables SDL2 Events, we can OR SDL_INIT_AUDIO and SDL_INIT_TIMER if needed in the future
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL2: %s", SDL_GetError());
        return EXIT_FAILURE;
	}

	// Create a 640 x 320 (px) window
	m_window = SDL_CreateWindow("CCHIP8 (SDL2)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							  (CHIP8_COLUMNS * 10), (CHIP8_ROWS * 10), SDL_WINDOW_SHOWN);

	// Check if Window could be crafted
	if (m_window == NULL)
	{
        printf("Could not create SDL2 Window: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

	// Set screen as a pointer to the window's surface
	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	// Check if renderer could be crafted
	if (m_renderer == NULL)
	{
        printf("Could not create SDL2 Renderer: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

	// Adjust the renderer size
	SDL_RenderSetLogicalSize(m_renderer, (CHIP8_COLUMNS * 10), (CHIP8_ROWS * 10));
	
	// Setup the texture trick that'll enable us to display emulator output
	m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CHIP8_COLUMNS, CHIP8_ROWS);

	// Check if texture could not be made
	if (m_texture == NULL)
	{
		printf("Could not create SDL2 Texture: %s\n", SDL_GetError());
	}

	// Set SDL2 Icon using RAW Data Method by blog.gibson.sh
	/*
		NOTE:
		Windows: Icon gets displayed as a Window Icon. Taskbar icon needs to be handled in some other way.
		Linux (Ubuntu-WSL): Icon gets displayed in the taskbar but not in the window icon.

		If we use this method for setting the icon, we can skip over including another SDL2 dependency
		(SDL2_image) and write the RAW Icon data inside the program. 
	*/
	SDL_SetWindowIconFromRAW(m_window);

	// Update the framebuffer with all the changes
	SDL_UpdateWindowSurface(m_window);

	// Create an SDL2 event
	SDL_Event m_event;

	while (true)
	{
		// Use a while() block waiting for SDL_PollEvent to intercept keyboard and sound events
		while (SDL_PollEvent(&m_event))
		{
			switch (m_event.type)
			{
				case SDL_QUIT:
					// Delist the Texture
					m_texture = NULL;

					// Deallocate the Texture
					SDL_DestroyTexture(m_texture);

					// Delist the Renderer
					m_renderer = NULL;

					// Deallocate the Renderer
					SDL_DestroyRenderer(m_renderer);

					// Delist the Window
					m_window = NULL;

					// Deallocate the Window
					SDL_DestroyWindow(m_window);

					// Close all SDL2 Subsystems
					SDL_Quit();

					// Exit the program successfully
					exit(EXIT_SUCCESS);
					
					// End case SDL_QUIT
					break;

				case SDL_KEYDOWN:
					// Check if debug mode is enabled
					if (m_dbgmode == true)
					{
						m_exec(&chip8);

						printf("\n\nCurrent OP: 0x%X\n", chip8.m_currentopcode);

						for (size_t i = 0; i < 16; i++)
						{
							printf("V Reg %zu: 0x%X\n",i , chip8.m_registers[i]);
						}

						printf("Index Reg: 0x%X\n", chip8.m_index);
						printf("PC Reg: 0x%X\n", chip8.m_programcounter);
						printf("SP Reg: 0x%X\n", chip8.m_stackp);
						printf("Delay Timer Reg: 0x%X\n", chip8.m_delaytmr);
						printf("Sound Timer Reg: 0x%X\n", chip8.m_soundtmr);
						break;
					}

					for (size_t i = 0; i < CHIP8_KEYS; i++)
					{
						if (m_event.key.keysym.sym == m_sdl_keys[i])
						{
							chip8.m_keyboard[i] = 1;
						}
            		}

					// End case SDL_KEYDOWN
					break;

				case SDL_KEYUP:
					// Check if debug mode isn't enabled
					if (m_dbgmode == false)
					{
						for (size_t i = 0; i < CHIP8_KEYS; i++)
						{
							if (m_event.key.keysym.sym == m_sdl_keys[i])
							{
								chip8.m_keyboard[i] = 0;
							}
						}
					}
					// End if m_dbgmode... statement
					break;
									
				// End m_event switch() statement
				default:
					break;
			}
		}
		
		// Once SDL's PollEvent while() has returned, continue with execution
		
		/*
			Check if opcode is unimplemented, if true, exit the main emulator loop (Fetch & Decode),
			clear the SDL2 surface, close the GUI window and quit SDL2 altogether.
		*/
		if (chip8.m_isUnimplemented == true)
		{
			if (m_no_exit == true)
			{
				while (true)
				{
					while (SDL_PollEvent(&m_event))
					{
						switch (m_event.type)
						{
							case SDL_QUIT:
								SDL_Quit();
								exit(EXIT_FAILURE);

							default:
								break;
						}	
					}
				}
			} else {
				printf("Exiting the main loop...\n");

				// Delist the Texture
				m_texture = NULL;

				// Deallocate the Texture
				SDL_DestroyTexture(m_texture);

				// Delist the Renderer
				m_renderer = NULL;

				// Deallocate the Renderer
				SDL_DestroyRenderer(m_renderer);

				// Delist the Window
				m_window = NULL;

				// Deallocate the Window
				SDL_DestroyWindow(m_window);

				// Close all SDL2 Subsystems
				SDL_Quit();

				// Exit the program returning a failure
				exit(EXIT_FAILURE);
			}
		} else {
			if (m_dbgmode == false)
			{
				// Execute the fetch & decode
				m_exec(&chip8);
			}
		}

		if (chip8.m_redraw)
		{
			chip8.m_redraw = false;

			SDL_UpdateTexture(m_texture, NULL, chip8.m_display, 64 * sizeof(uint32_t));
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

		if (m_dbgmode == false)
		{

#if defined(__MINGW32__) || defined(__MINGW64__)
			/*
				NOTE:
				mingw complains about using usleep, _sleep is deprecated so we *should*
				use <windows.h>'s Sleep() function instead, investigate how it works and apply
				a good fix.
			*/
			usleep(1500);
#endif
#ifdef __unix__
			struct timespec time1, time2;
			time1.tv_sec = 0;
			time1.tv_nsec = 1500000;
			nanosleep(&time1, &time2);
#endif
		}
	}
}

// SDL2 Icon using RAW Data Method by blog.gibson.sh
void SDL_SetWindowIconFromRAW(SDL_Window* m_window)
{
  #include "cchip8_icn.h"

  uint32_t rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  int shift = (m_cchip8_icn.bytes_per_pixel == 3) ? 8 : 0;
  rmask = 0xff000000 >> shift;
  gmask = 0x00ff0000 >> shift;
  bmask = 0x0000ff00 >> shift;
  amask = 0x000000ff >> shift;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = (m_cchip8_icn.bytes_per_pixel == 3) ? 0 : 0xff000000;
#endif

  SDL_Surface* m_icon = SDL_CreateRGBSurfaceFrom((void*)m_cchip8_icn.pixel_data,
      m_cchip8_icn.width, m_cchip8_icn.height, m_cchip8_icn.bytes_per_pixel*8,
      m_cchip8_icn.bytes_per_pixel*m_cchip8_icn.width, rmask, gmask, bmask, amask);

  // Check if surface could not be made
  if (m_icon == NULL)
  {
  	printf("Could not create SDL2 Surface: %s\n", SDL_GetError());
  }

  SDL_SetWindowIcon(m_window, m_icon);

  SDL_FreeSurface(m_icon);
}