#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "setjmp.h"

#include "SDL.h"
#undef main

#define TRY do{ jmp_buf ex_buf__; if( !setjmp(ex_buf__) )}
#define CATCH } else {
#define ETRY } }while(0)
#define THROW longjmp(ex_buf__, 1);

// 4096 bytes of RAM
// 0x000 to 0x1FF reserved for interpreter
// 0x200 start of most programs
// 0x600 start of ETI 660 programs
// program space til 0xFFF
uint8_t* memoryMap;

// 16 8-bit registers
// vF should not be used as it is used as a flag for instructions
uint8_t* V;

// 1 16-bit register
// used to store memory addresses, only last 12 bits are used usually
uint16_t* I;

// 16-bit program counter
uint16_t* PC;

// 8-bit stack pointer
int8_t* SP;

// 16 16-bit memory space for memory return addresses
uint16_t* stack;

// 16-bit of keyboard
uint16_t* keyboard;

#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32



void dump_memory()
{
	printf("MEMORY:\n");
	for (uint16_t PC = 0x000; PC < 0x1000; PC++)
	{
		printf("%02X ", memoryMap[PC]); // print in hex
		if ((PC + 1) % 16 == 0) printf("\n"); // print new line every 16 bytes
	}
	printf("\n");

	printf("V Registers:\n");
	for (uint16_t v_ = 0x00; v_ < 0x10; v_++)
	{
		printf("%02X ", V[v_]); // print in hex
	}
	printf("\n");
}

int main(int argc, char** argv)
{
	memoryMap = calloc((size_t)4096, sizeof(uint8_t)); // 4096 8-bit words
	if (memoryMap == NULL)
		return -1;
	V = calloc((size_t)16, sizeof(uint8_t)); // 16 16-bit registers
	if (V == NULL)
		return -1;
	I = malloc(sizeof(uint16_t)); // 16-bit
	if (I == NULL)
		return -1;
	PC = malloc(sizeof(uint16_t)); // 16-bit
	if (PC == NULL)
		return -1;
	SP = malloc(sizeof(int8_t)); // 8-bit
	if (SP == NULL)
		return -1;
	*SP = -(0x01); // set stack pointer to 0
	stack = calloc((size_t)16, sizeof(uint8_t)); // 16 16-bit memory addresses
	if (stack == NULL)
		return -1;
	keyboard = malloc(sizeof(int16_t));
	if (keyboard == NULL)
		return -1;
	
	// load program into memory
	FILE* file;
	uint8_t* buffer;
	size_t buffer_size = 4096;
	size_t bytes_read;

	file = fopen("G:\\Brians CHIP8\\Brians CHIP8\\1-chip8-logo.ch8", "rb");
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

	memcpy(memoryMap + 0x200, buffer, bytes_read); // copy file buffer into memory map starting at 0x200

	free(buffer); // free file reading memory

	for (size_t i = 0; i < bytes_read; i++)
	{
		printf("%02X ", memoryMap[i + 0x200]); // print in hex
		if ((i + 1) % 16 == 0) printf("\n"); // print new line every 16 bytes
	}
	printf("\n");


	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int i = 0; i < WINDOW_WIDTH; i++)
	{
		SDL_RenderDrawPoint(renderer, i, i);
	}
	SDL_RenderPresent(renderer);

	// start at 0x200
	*PC = 0x200;


	while (*PC != 0xFFF)
	{
		// op is MSB first and two words
		uint16_t op = ((uint16_t)memoryMap[*PC] << 0x8) | (memoryMap[*PC + 0x001]); // MSB shift right 8 times to fill upper 8 bits, then get next byte for lower 8 bits
		printf("op: %02X\n", op);
		switch (op)
		{
			// CLS - clear the display
		case (0x00E0):
			break;

			// RET - return from subroutine
		case (0x00EE):
			*PC = stack[*SP]; // set program counter to address on stack
			*SP -= 0x01; // decrement stack pointer by 1;
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
			printf("JP\n");
			*PC = op & 0xF000; // set PC to address. &0x0111 is a mask to only get last three bits of opcode
			continue;
		}
		// 2nnn - CALL addr - call subroutine at nnn
		else if ((op & 0xF000) == 0x2000)
		{
			printf("CALL\n");
			*SP += 0x01; // increment the stack pointer
			stack[*SP] = *PC; // put PC on stack for return address
			*PC = 0x0111 & op; // set PC to subroutine address. 0x0111 is a mask to only get last three bits of opcode
		}
		// 3xkk - SE Vx, byte - skip next instrcution if Vx = kk
		else if ((op & 0xF000) == 0x3000)
		{
			printf("SE\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get x by masking first and last two digits, then shift it right a byte to get single 16-bit index
			uint8_t kk = op & 0x00FF; // mask front byte to get kk

			if (Vx == kk) // if register is the same as value
				*PC += 0x002; // increment PC to skip

		}
		// 4xkk - SNE Vx, byte - skip next instruction if Vx != kk
		else if ((op & 0xF000) == 0x4000)
		{
			printf("SNE\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get x by masking first and last two digits, then shift it right a byte to get single 16-bit index
			uint8_t kk = op & 0x00FF; // maske first byte to get kk

			if (Vx != kk)
				*PC += 0x002; // increment PC to skip

		}
		// 5xy0 - SE Vx, Vy - skip next instruction if Vx != Vy
		else if ((op & 0xF000) == 0x5000)
		{
			printf("SE\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get x by masking first 4 bits and last byte then shifting over one byte
			uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking first byte and last 4 bits then shifting over 4 bits

			if (Vx == Vy) // check if registers are equal
				*PC += 0x002; // increment PC to skip
		}
		// 6xkk - LD Vx, kk byte
		else if ((op & 0xF000) == 0x6000)
		{
			printf("LD\n");
			uint8_t kk = op & 0x00FF; // mask first byte to get kk
			V[(op & 0x0100) >> 0x8] = kk; // access Vk by masking first 4 bits and last byte then shifting over one byte
		}
		// 7xkk - ADD Vx, byte
		else if ((op & 0xF000) == 0x7000)
		{
			printf("ADD\n");
			uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and last byte then shifting right one byte
			uint8_t kk = op & 0x00FF; // get kk by masking first byte

			*Vx = *Vx + kk; // Vx += kk
		}
		// 8 bitwise instructions
		else if ((op & 0xF000) == 0x8000)
		{
			// 8xy0 - LD Vx, Vy - Set Vx = Vy
			if ((op & 0xF000) == 0x8001) // check for ending 0
			{
				printf("LD Vy\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vk = V[(op & 0x0010) >> 0x4];
			}
			// 8xy1 - OR Vx, Vy - Set Vx = Vx Bit-OR Vy
			else if ((op & 0xF000) == 0x8001)
			{
				printf("OR\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking higher byte and lower 4 bits
				*Vx = *Vx | Vy; // Vx = Vx or Vy
			}
			// 8xy2 - AND Vx, Vy - Set Vx = Vx Bit-AND Vy
			else if ((op & 0xF000) == 0x8002)
			{
				printf("AND\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking higher byte and lower 4 bits
				*Vx = *Vx & Vy; // Vx = Vx AND Vy
			}
			// 8xy3 - XOR Vx, Vy - Set Vx = Vx Bit-XOR Vy
			else if ((op & 0xF000) == 0x8003)
			{
				printf("XOR\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking higher byte and lower 4 bits
				*Vx = *Vx ^ Vy; // Vx = Vx Bit-XOR Vy
			}
			// 8xy4 - ADD Vx, Vy - Set Vx = Vx + Vy, Set VF as carry bit
			else if ((op & 0xF000) == 0x8004)
			{
				printf("ADD\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking higher byte and lower 4 bits
				uint16_t sum = *Vx + Vy; // add into a u16;
				uint8_t carryBit = sum & 0x0100; // mask all bits except carry bit
				V[0xF] = carryBit; // store carry bit in VF
			}
			// 8xy5 - SUB Vx, Vy - Set Vx = Vx - Vy, Set VF = NOT borrow
			else if ((op & 0xF000) == 0x8005)
			{
				printf("SUB\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking first 4 bits and lower byte then shifting right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get y by masking higher byte and lower 4 bits
				if (*Vx > Vy) // if Vx is greater than Vy
					V[0xF] = 0x01; // Set VF to 1
				*Vx -= Vy; // Subtract Vy from Vx, store in Vx
			}
			// 8xy6 - SHR Vx {, Vy} - Set Vx = Vx SHR 1
			else if ((op & 0xF000) == 0x8006)
			{
				printf("SHR\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get pointer to x by masking higher 4 bits and lower byte then shifting right one byte
				V[0xF] = *Vx & 0x01; // mask highest 7 bits then store in VF
				*Vx /= 0x02; // divide by 
			}
			// 8xy7 - SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT BORROW
			else if ((op & 0xF000) == 0x8007)
			{
				printf("SUBN\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
				uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get pointer to Vy by masking highest byte and lowest 4 bits then shift right 4 bits
				if (Vy > *Vx) // If Vy is larger than Vx
					V[0xF] = 0x01; // set VF to true
				Vx = Vy - *Vx; // Subtract Vx from Vy, store result in Vx;
			}
			// 8xyE - SHL Vx {, Vy}
			else if ((op & 0xF000) == 0x800E)
			{
				printf("SHL\n");
				uint8_t* Vx = &V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
				V[0xF] = *Vx & 0x07 >> 0xF; // mask to get MSB, shift right 7 bits to get in lowest position, store in VF
				*Vx *= 0x02; // set Vx = Vx * 2
			}
		}
		// 9xy0 - SNE Vx, Vy - skip next instruction if Vx != Vy
		else if ((op & 0xF000) == 0x9000)
		{
			printf("SNE\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
			uint8_t Vy = V[(op & 0x0010) >> 0x4]; // get Vy by masking highest byte and lowest 4 bits then shift right 4 bits
			if (Vx != Vy) // if values of Vx != Vy
				*PC += 0x002; // skip next instruction
		}
		// Annn - LD I, addr - The value of register I is set to nnn
		else if ((op & 0xF000) == 0xA000)
		{
			printf("LD I\n");
			*I = op & 0x0FFF;
		}
		// Bnnn - JP V0, addr - Set PC to addr + V0
		else if ((op & 0xF000) == 0xB000)
		{
			printf("JP V0\n");
			*PC = (op & 0x0111) + V[0x0]; // set PC to addr + V0
		}
		// Cxkk - RND Vx, byte - Set Vx = random byte AND kk
		else if ((op & 0xF000) == 0xC000)
		{
			printf("RND\n");
			uint8_t* Vx = V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shifting right one byte
			uint8_t kk = op & 0x00FF; // mask highest byte to get kk
			*Vx = kk & (rand() % 0x100); // random num between 0 and 255
		}
		// Dxyn - DRW Vx, Vy, n - display n-bytes sprite starting at memory location I at (Vx, Vy), set VF = collision
		else if ((op & 0xF000) == 0xD000)
		{
			printf("DRW\n");
			// read n bytes from memory starting at addr I. Display as sprites at coordinates (Vx, Vy). Sprites are XORed onto the existing screen space.
			// if this causes any pixels to be erased, VF is set to 1 otherwise 0.
			// if the sprite is positioned so part of it is outside the coords of the display, it wraps aroudn the opposite side of the screen.
		}
		// Ex9E - SKP Vx - Skip the next instruction if key with the value of Vx is pressed
		else if ((op & 0xF000) == 0xE09E)
		{
			printf("SKP\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
			if (((*keyboard >> Vx) & 0xFE) == 0x01) // shift right by enough bits to get the specific key then mask every other bit. if 1 then
				*PC += 0x002; // increment PC by one opcode
		}
		// ExA1 - SKNP Vx - Skip next instruction if key with the value of Vx is not pressed
		else if ((op & 0xF000) == 0xE0A1)
		{
			printf("SKNP\n");
			uint8_t Vx = V[(op & 0x0100) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
			if (((*keyboard >> Vx) & 0xFE) == 0x00) // shift right by enough bits to get the specific key then mask every other bit. if 0 then
				*PC += 0x002; // increment PC by one opcode
		}
		// 0xF--- fall into the F opcodes
		else if ((op & 0xF000) == 0xF000)
		{
			printf("F\n");
		}
		else
		{
			// unsupported opcode
			printf("UNSUPPORTED OPCODE treated as no op\n");
		}

		if (*PC != 0xFFE)
			*PC += 0x002; // increment program counter by one opcode
		else
			*PC = 0xFFF;

		if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
			break;
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	dump_memory();

	return 0;
}