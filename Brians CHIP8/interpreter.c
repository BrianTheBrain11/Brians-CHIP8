#include "interpreter.h"

#include <stdio.h>
#include <memory.h>
#include "screen_manager.h"

void handle_1(uint16_t op, Chip8Context* context)
{
	context->PC = op & 0x0FFF; // set PC to address. &0x0111 is a mask to only get last three bits of opcode
	DEBUG_PRINT("JP to address %x\n", op & 0x0FFF);
}

void handle_A(uint16_t op, Chip8Context* context)
{
	*context->I = op & 0x0FFF;
	DEBUG_PRINT("LD I with value 0x%x\n", op & 0x0FFF);
}

void handle_D(uint16_t op, Chip8Context* context)
{
	// get starting address of sprite
	uint16_t start_address = *context->I;


	printf("Registers for coords: %x, %x\n", ((op & 0x0F00) >> 0x8), ((op & 0x00F0) >> 0x4));
	// get Vx, Vy
	uint8_t Vx = context->V[(op & 0x0F00) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shifting right one byte
	uint8_t Vy = context->V[(op & 0x00F0) >> 0x4]; // get Vx by masking highest 4 bits and lowest byte then shifting right one byte
	SpritePosition sprite_position = { Vx, Vy };

	// get n bytes to read and stroes it in sprite
	size_t sprite_length = (op & 0x000F);
	uint8_t* sprite = calloc((size_t)sprite_length, sizeof(uint8_t));
	if (sprite == NULL)
		return;

	memcpy(sprite, context->memoryMap + start_address, sprite_length);

	DEBUG_PRINT("SPRITE TO DRAW\n");
	for (int i = 0; i < sprite_length; i++)
	{
		uint8_t debug_row = sprite[i];
		for (int bit = 0; bit < 8; bit++)
		{
			if (debug_row & 0x80)
				printf("1");
			else
				printf("0");

			debug_row <<= 1;
		}
		printf("\n");
	}
	// draw it to screen now
	draw(sprite, sprite_length, sprite_position, context);

	DEBUG_PRINT("DRW at %d, %d, 0x%x bytes from 0x%x\n", Vx, Vy, (unsigned int)sprite_length, start_address);
}
