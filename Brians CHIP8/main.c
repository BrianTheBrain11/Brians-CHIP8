#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"
#include <stdbool.h>

#include "Chip8Context.h"
#include "interpreter_loader.h"
#include "interpreter.h"

#include <SDL.h>
#undef main

#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) )}
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1);



#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32


Chip8Context context;

void sdl_frame_cleanup(bool* running)
{
	SDL_RenderPresent(context.renderer);

	while (SDL_PollEvent(&context.event))
	{
		if (context.event.type == SDL_QUIT)
		{
			*running = false;
		}
	}
}

void dump_memory()
{
	printf("MEMORY:\n");
	for (uint16_t PC = 0x000; PC < 0x1000; PC++)
	{
		printf("%02X ", context.memoryMap[PC]); // print in hex
		if ((PC + 1) % 16 == 0) printf("\n"); // print new line every 16 bytes
	}
	printf("\n");

	printf("Registers:\n");
	for (uint16_t v_ = 0x00; v_ < 0x10; v_++)
	{
		printf("%02X ", context.V[v_]); // print in hex
	}
	printf("\n");

	printf("PC: %02X\n", context.PC);
	printf("SP: %02X\n", context.SP);
	printf("\n");
}

void increment_pc()
{
	if (context.PC != 0xFFE)
		context.PC += 0x002; // increment program counter by one opcode
	else
		context.PC = 0xFFF;
}

int main(int argc, char** argv)
{

	context.memoryMap = calloc((size_t)4096, sizeof(uint8_t)); // 4096 8-bit words
	if (context.memoryMap == NULL)
		return -1;
	context.V = calloc((size_t)16, sizeof(uint8_t)); // 16 16-bit registers
	if (context.V == NULL)
		return -1;
	context.I = 0;
	context.stack = calloc((size_t)16, sizeof(uint8_t)); // 16 16-bit memory addresses
	if (context.stack == NULL)
		return -1;
	context.SP = 0;
	context.keyboard = malloc(sizeof(int16_t));
	if (context.keyboard == NULL)
		return -1;
	context.video_memory_map = calloc((size_t)32, sizeof(uint8_t*)); // make 32 y columns
	if (context.video_memory_map == NULL)
		return -1;
	for (int y = 0; y < 32; y++)
	{
		context.video_memory_map[y] = calloc((size_t)8, sizeof(uint8_t));
		if (context.video_memory_map[y] == NULL)
			return -1;
	}

	// load interpeter data
	load_hex_sprites(context.memoryMap);
	
	// load program into memory
	FILE* file;
	uint8_t* buffer;
	size_t buffer_size = 4096;
	size_t bytes_read;

	file = fopen("G:\\Brians CHIP8\\chip8-test-suite\\bin\\4-flags.ch8", "rb");

	if (file == NULL)
	{
		perror("Failed to open file");
		return -1;
	}

	buffer = malloc(buffer_size);
	if (buffer == NULL)
	{
		fclose(file);
		return -1;
	}

	bytes_read = fread(buffer, 1, buffer_size, file);
	if (bytes_read == 0 && ferror(file))
	{
		perror("Error reading file");
		free(buffer);
		fclose(file);
		return -1;
	}

	memcpy(context.memoryMap + 0x200, buffer, bytes_read); // copy file buffer into memory map starting at 0x200

	free(buffer); // free file reading memory

	for (size_t i = 0; i < bytes_read; i++)
	{
		printf("%02X ", context.memoryMap[i + 0x200]); // print in hex
		if ((i + 1) % 16 == 0) printf("\n"); // print new line every 16 bytes
	}
	printf("\n");


	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH * 10, WINDOW_HEIGHT * 10, 0, &context.window, &context.renderer);
	SDL_RenderSetLogicalSize(context.renderer, WINDOW_WIDTH, WINDOW_HEIGHT);
	SDL_SetRenderDrawColor(context.renderer, 0, 0, 0, 0);
	SDL_RenderClear(context.renderer);

	for (int i = 0; i < WINDOW_WIDTH; i++)
	{
		SDL_RenderDrawPoint(context.renderer, i, i);
	}

	SDL_RenderPresent(context.renderer);

	// start at 0x200
	context.PC = 0x200;

	bool run;
	bool running = true;

#ifdef _DEBUG
	run = false;
#else
	run = true;
#endif

	uint32_t lastTime = SDL_GetTicks();

	while (running) // main program loop
	{
		uint32_t frameStart = SDL_GetTicks();

		if (frameStart - lastTime < 5)
			continue;

		lastTime = frameStart;

		if (run == false)
		{
			char input[50];

			scanf("%49s", input);

			if (strcmp(input, "dump") == 0)
			{
				dump_memory();
				continue;
			}
			if (strcmp(input, "run") == 0)
			{
				run = true;
				continue;
			}
			else if (strcmp(input, "step") == 0)
			{
				// dont do anything, just execute the next loop
			}
		}

		if (context.PC != 0xFFF)
		{
			// op is MSB first and two words
			uint16_t op = ((uint16_t)context.memoryMap[context.PC] << 0x8) | (context.memoryMap[context.PC + 0x001]); // MSB shift right 8 times to fill upper 8 bits, then get next byte for lower 8 bits
			DEBUG_PRINT("op: %02X, PC: %x\n", op, context.PC);
			switch (op)
			{
				// CLS - clear the display
			case (0x00E0):
				DEBUG_PRINT("Clearing the display with 00E0\n");
				clear_display(context);
				increment_pc();
				sdl_frame_cleanup(&running);
				continue;
				break;

				// RET - return from subroutine
			case (0x00EE):
				context.SP -= 1; // decrement stack pointer by 1;
				context.PC = context.stack[context.SP]; // set program counter to address on stack
				DEBUG_PRINT("RET from subroutine. PC value to return to: %x", context.stack[context.SP]);
				increment_pc();
				sdl_frame_cleanup(&running);
				continue;
				break;

				// an opcode with an address or an unsupported opcode
			default:
				break;
			}

			// 0nnn - SYS addr - jump to machine code routine at nnn -- no op in modern interpreters 

			if (0) // check if all 0 todo: fix
			{
				printf("falling in here for some reason\n");
			}
			// 1nnn - JP addr - jump to location nnn
			else if ((op & 0xF000) == 0x1000)
			{
				handle_1(op, &context);
				sdl_frame_cleanup(&running);
				continue;
			}
			// 2nnn - CALL addr - call subroutine at nnn
			else if ((op & 0xF000) == 0x2000)
			{
				handle_2(op, &context);
				sdl_frame_cleanup(&running);
				continue;
			}
			// 3xkk - SE Vx, byte - skip next instrcution if Vx = kk
			else if ((op & 0xF000) == 0x3000)
			{
				handle_3(op, &context);
			}
			// 4xkk - SNE Vx, byte - skip next instruction if Vx != kk
			else if ((op & 0xF000) == 0x4000)
			{
				handle_4(op, &context);
			}
			// 5xy0 - SE Vx, Vy - skip next instruction if Vx != Vy
			else if ((op & 0xF000) == 0x5000)
			{
				handle_5(op, &context);
			}
			// 6xkk - LD Vx, kk byte
			else if ((op & 0xF000) == 0x6000)
			{
				uint8_t kk = op & 0x00FF; // mask first byte to get kk
				context.V[(op & 0x0F00) >> 0x8] = kk; // access Vk by masking first 4 bits and last byte then shifting over one byte")
				DEBUG_PRINT("LD V%d with value 0x%x\n", (op & 0x0F00) >> 0x8, kk);
			}
			// 7xkk - ADD Vx, byte
			else if ((op & 0xF000) == 0x7000)
			{
				handle_7(op, &context);
			}
			// 8 bitwise instructions
			else if ((op & 0xF000) == 0x8000)
			{
				// 8xy0 - LD Vx, Vy - Set Vx = Vy
				if ((op & 0xF00F) == 0x8000) // check for ending 0
				{
					printf("LD Vy\n");
					uint8_t* Vx = &context.V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4];
					*Vx = Vy;
				}
				// 8xy1 - OR Vx, Vy - Set Vx = Vx Bit-OR Vy
				else if ((op & 0xF00F) == 0x8001)
				{
					printf("OR\n");
					uint8_t* Vx = &context.V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get y by masking higher byte and lower 4 bits
					*Vx = *Vx | Vy; // Vx = Vx or Vy
				}
				// 8xy2 - AND Vx, Vy - Set Vx = Vx Bit-AND Vy
				else if ((op & 0xF00F) == 0x8002)
				{
					printf("AND\n");
					uint8_t* Vx = &context.V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get y by masking higher byte and lower 4 bits
					*Vx = *Vx & Vy; // Vx = Vx AND Vy
				}
				// 8xy3 - XOR Vx, Vy - Set Vx = Vx Bit-XOR Vy
				else if ((op & 0xF00F) == 0x8003)
				{
					printf("XOR\n");
					uint8_t* Vx = &context.V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get y by masking higher byte and lower 4 bits
					*Vx = *Vx ^ Vy; // Vx = Vx Bit-XOR Vy
				}
				// 8xy4 - ADD Vx, Vy - Set Vx = Vx + Vy, Set VF as carry bit
				else if ((op & 0xF00F) == 0x8004)
				{
					uint8_t* Vx = &context.V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get y by masking higher byte and lower 4 bits
					uint16_t sum = *Vx + Vy; // add into a u16;
					uint8_t carryBit = 0;
					if (sum > 255)
					{
						carryBit = 1;
						sum %= 256;
					}
					*Vx = sum;
					context.V[0xF] = carryBit; // store carry bit in VF
					DEBUG_PRINT("ADD, carry bit: %x\n", carryBit);
				}
				// 8xy5 - SUB Vx, Vy - Set Vx = Vx - Vy, Set VF = NOT borrow
				else if ((op & 0xF00F) == 0x8005)
				{
					uint8_t x = ((op & 0x0F00) >> 0x8);
					uint8_t Vx = context.V[x]; // get x by masking first 4 bits and lower byte then shifting right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get y by masking higher byte and lower 4 bits

					context.V[x] -= Vy; // Subtract Vy from Vx, store in Vx

					context.V[0xF] = 0x00;
					if (Vx >= Vy) // if Vx is greater than Vy
						context.V[0xF] = 0x01; // Set VF to 1

					DEBUG_PRINT("SUB, borrow bit: %x\n", context.V[0xF]);
				}
				// 8xy6 - SHR Vx {, Vy} - Set Vx = Vx SHR 1
				else if ((op & 0xF00F) == 0x8006)
				{
					printf("SHR\n");
					uint8_t x = ((op & 0x0F00) >> 0x8);
					uint8_t Vx = context.V[x]; // get pointer to x by masking higher 4 bits and lower byte then shifting right one byte
					context.V[x] /= 0x02; // divide by 2
					context.V[0xF] = Vx & 0x01; // mask highest 7 bits then store in VF
				}
				// 8xy7 - SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT BORROW
				else if ((op & 0xF00F) == 0x8007)
				{
					printf("SUBN\n");
					uint8_t x = ((op & 0x0F00) >> 0x8);
					uint8_t Vx = context.V[x]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
					uint8_t Vy = context.V[(op & 0x00F0) >> 0x4]; // get pointer to Vy by masking highest byte and lowest 4 bits then shift right 4 bits
					Vx = Vy - Vx; // Subtract Vx from Vy, store result in Vx;

					context.V[x] = Vx;

					context.V[0xF] = 0x00;
					if (Vy >= Vx) // If Vy is larger than Vx
						context.V[0xF] = 0x01; // set VF to true
				}
				// 8xyE - SHL Vx {, Vy}
				else if ((op & 0xF00F) == 0x800E)
				{
					printf("SHL\n");
					uint8_t x = ((op & 0x0F00) >> 0x8);
					uint8_t Vx = context.V[x]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
					context.V[x] *= 0x02; // set Vx = Vx * 2
					context.V[0xF] = (Vx & 0x80) >> 0x7; // mask to get MSB, shift right 7 bits to get in lowest position, store in VF
				}
			}
			// 9xy0 - SNE Vx, Vy - skip next instruction if Vx != Vy
			else if ((op & 0xF000) == 0x9000)
			{
				handle_9(op, &context);
			}
			// Annn - LD I, addr - The value of register I is set to nnn
			else if ((op & 0xF000) == 0xA000)
			{
				handle_A(op, &context);
			}
			// Bnnn - JP V0, addr - Set PC to addr + V0
			else if ((op & 0xF000) == 0xB000)
			{
				printf("JP V0\n");
				context.PC = (op & 0x0111) + context.V[0x0]; // set PC to addr + V0
			}
			// Cxkk - RND Vx, byte - Set Vx = random byte AND kk
			else if ((op & 0xF000) == 0xC000)
			{
				printf("RND\n");
				uint8_t* Vx = &context.V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shifting right one byte
				uint8_t kk = op & 0x00FF; // mask highest byte to get kk
				*Vx = kk & (rand() % 0x100); // random num between 0 and 255
			}
			// Dxyn - DRW Vx, Vy, n - display n-bytes sprite starting at memory location I at (Vx, Vy), set VF = collision
			else if ((op & 0xF000) == 0xD000)
			{
				handle_D(op, &context);
			}
			// Ex9E - SKP Vx - Skip the next instruction if key with the value of Vx is pressed
			else if ((op & 0xF000) == 0xE09E)
			{
				printf("SKP\n");
				uint8_t Vx = context.V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
				if (((*context.keyboard >> Vx) & 0xFE) == 0x01) // shift right by enough bits to get the specific key then mask every other bit. if 1 then
					context.PC += 0x002; // increment PC by one opcode
			}
			// ExA1 - SKNP Vx - Skip next instruction if key with the value of Vx is not pressed
			else if ((op & 0xF000) == 0xE0A1)
			{
				printf("SKNP\n");
				uint8_t Vx = context.V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
				if (((*context.keyboard >> Vx) & 0xFE) == 0x00) // shift right by enough bits to get the specific key then mask every other bit. if 0 then
					context.PC += 0x002; // increment PC by one opcode
			}
			// 0xF--- fall into the F opcodes
			else if ((op & 0xF000) == 0xF000)
			{
				if ((op & 0xF0FF) == 0xF01E)
				{
					handle_F1E(op, &context);
				}
				else if ((op & 0xF0FF) == 0xF033)
				{
					handle_F33(op, &context);
				}
				else if ((op & 0xF0FF) == 0xF055)
				{
					handle_F55(op, &context);
				}
				else if ((op & 0xF0FF) == 0xF065)
				{
					handle_F65(op, &context);
				}
			}
			else
			{
				// unsupported opcode
				printf("UNSUPPORTED OPCODE treated as no op\n");
			}

			increment_pc();

			SDL_RenderPresent(context.renderer);

			while (SDL_PollEvent(&context.event))
			{
				if (context.event.type == SDL_QUIT)
				{
					running = false;
				}
			}
		}
	}

	SDL_DestroyRenderer(context.renderer);
	SDL_DestroyWindow(context.window);
	SDL_Quit();

	//dump_memory();

	return 0;
}