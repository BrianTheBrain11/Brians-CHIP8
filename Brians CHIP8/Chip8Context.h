#pragma once

#include <stdint.h>
#include <SDL.h>

typedef struct
{
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
	uint16_t I;

	// 16-bit program counter
	uint16_t PC;

	// 8-bit stack pointer
	int8_t SP;

	// 16 16-bit memory space for memory return addresses
	uint16_t* stack;

	// 16-bit of keyboard
	uint16_t* keyboard;

	// 64-bit * 32-bit of RAM for display output -- essentially 256 bytes of mem
	// self chosen to be 8 bytes across, 4 bytes down
	uint8_t** video_memory_map; // access with video_memory_map[y][x] up to [31][63]

	// sdl handles
	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;

} Chip8Context;