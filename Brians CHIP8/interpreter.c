#include "interpreter.h"

#include <stdio.h>
#include <memory.h>
#include <SDL.h>
#include "screen_manager.h"

void handle_1(uint16_t op, Chip8Context* context)
{
	context->PC = op & 0x0FFF; // set PC to address. &0x0111 is a mask to only get last three bits of opcode
	DEBUG_PRINT("JP to address %x\n", op & 0x0FFF);
}

void handle_2(uint16_t op, Chip8Context* context)
{
	context->stack[context->SP] = context->PC; // put PC on stack for return address
	context->SP += 1; // increment the stack pointer
	context->PC = 0x0FFF & op; // set PC to subroutine address. 0x0111 is a mask to only get last three bits of opcode
	DEBUG_PRINT("CALL subroutine at addr: %x\n", 0x0FFF & op);
}

void handle_3(uint16_t op, Chip8Context* context)
{
	uint8_t Vx = context->V[(op & 0x0F00) >> 0x8]; // get x by masking first and last two digits, then shift it right a byte to get single 16-bit index
	uint8_t kk = op & 0x00FF; // mask front byte to get kk

	DEBUG_PRINT("SE: skip if Register V%x: %x equals value: %x\n", (op & 0x0F00) >> 0x8, Vx, kk);
	if (Vx == kk) // if register is the same as value
		context->PC += 0x002; // increment PC to skip
}

void handle_4(uint16_t op, Chip8Context* context)
{
	uint8_t Vx = context->V[(op & 0x0F00) >> 0x8]; // get x by masking first and last two digits, then shift it right a byte to get single 16-bit index
	uint8_t kk = op & 0x00FF; // maske first byte to get kk

	DEBUG_PRINT("SNE: skip if Register V%x: %x does not equal value: %x\n", (op & 0x0F00) >> 0x8, Vx, kk);

	if (Vx != kk)
		context->PC += 0x002; // increment PC to skip
}

void handle_5(uint16_t op, Chip8Context* context)
{
	uint8_t Vx = context->V[(op & 0x0F00) >> 0x8]; // get x by masking first 4 bits and last byte then shifting over one byte
	uint8_t Vy = context->V[(op & 0x00F0) >> 0x4]; // get y by masking first byte and last 4 bits then shifting over 4 bits

	DEBUG_PRINT("SE: skip if Register V%x: %x does equals Register V%x: %x\n", (op & 0x0F00) >> 0x8, Vx, (op & 0x00F0) >> 0x4, Vy);

	if (Vx == Vy) // check if registers are equal
		context->PC += 0x002; // increment PC to skip
}

void handle_7(uint16_t op, Chip8Context* context)
{
	uint8_t* Vx = &context->V[(op & 0x0F00) >> 0x8]; // get pointer to x by masking first 4 bits and last byte then shifting right one byte
	uint8_t kk = op & 0x00FF; // get kk by masking first byte

	*Vx = *Vx + kk; // Vx += kk

	DEBUG_PRINT("ADD V%x += %x", (op & 0x0F00) >> 0x8, kk);
}

void handle_9(uint16_t op, Chip8Context* context)
{
	uint8_t Vx = context->V[(op & 0x0F00) >> 0x8]; // get Vx by masking highest 4 bits and lowest byte then shift right one byte
	uint8_t Vy = context->V[(op & 0x00F0) >> 0x4]; // get Vy by masking highest byte and lowest 4 bits then shift right 4 bits

	DEBUG_PRINT("SNE: skip if Register V%x: %x does not equal Register V%x: %x\n", (op & 0x0F00) >> 0x8, Vx, (op & 0x00F0) >> 0x4, Vy);

	if (Vx != Vy) // if values of Vx != Vy
		context->PC += 0x002; // skip next instruction
}

void handle_A(uint16_t op, Chip8Context* context)
{
	context->I = op & 0x0FFF;
	DEBUG_PRINT("LD I with value 0x%x\n", op & 0x0FFF);
}

void handle_D(uint16_t op, Chip8Context* context)
{
	// get starting address of sprite
	uint16_t start_address = context->I;


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

void handle_F0A(uint16_t op, Chip8Context* context)
{
	DEBUG_PRINT("LD Vx, K");

	uint32_t lastTime = SDL_GetTicks();

	uint8_t* Vx = ((op & 0x0F00) >> 0x8);

	while (1)
	{
		uint32_t frameStart = SDL_GetTicks();
		if (frameStart - lastTime > INSTRUCTION_CYCLE)
		{
			while (SDL_PollEvent(&context->event))
			{
				if (context->event.type == SDL_KEYDOWN)
				{
					SDL_Keycode key = context->event.key.keysym.sym;  // Get the key code
					switch (key)
					{
					case SDLK_1:
						*Vx = 0x1;
						break;
					case SDLK_2:
						*Vx = 0x2;
						break;
					case SDLK_3:
						*Vx = 0x3;
						break;
					case SDLK_4:
						*Vx = 0xC;
						break;
					case SDLK_q:
						*Vx = 0x4;
						break;
					case SDLK_w:
						*Vx = 0x5;
						break;
					case SDLK_e:
						*Vx = 0x6;
						break;
					case SDLK_r:
						*Vx = 0xD;
						break;
					case SDLK_a:
						*Vx = 0x7;
						break;
					case SDLK_s:
						*Vx = 0x8;
						break;
					case SDLK_d:
						*Vx = 0x9;
						break;
					case SDLK_f:
						*Vx = 0xE;
						break;
					case SDLK_z:
						*Vx = 0xA;
						break;
					case SDLK_x:
						*Vx = 0x0;
						break;
					case SDLK_c:
						*Vx = 0xB;
						break;
					case SDLK_v:
						*Vx = 0xF;
						break;
					}

					continue;
				}
			}
			lastTime = frameStart;
		}
	}	
}

void handle_F1E(uint16_t op, Chip8Context* context)
{
	uint8_t x = ((op & 0x0F00) >> 0x8);
	uint8_t Vx = context->V[x];
	context->I += Vx;
	DEBUG_PRINT("ADD I += V%x\n", x);
}

void handle_F33(uint16_t op, Chip8Context* context)
{
	uint8_t x = ((op & 0x0F00) >> 0x8);
	uint8_t Vx = context->V[x];

	DEBUG_PRINT("LD register V%x into memory with BCD representation starting at I: %x", x, context->I);

	context->memoryMap[context->I] = Vx / 100;
	context->memoryMap[context->I + 1] = (Vx / 10) % 10;
	context->memoryMap[context->I + 2] = Vx % 10; // dividing by 1 doesn't do anything but for consistencies sake
}

void handle_F55(uint16_t op, Chip8Context* context)
{
	uint8_t finalRegisterIndex = ((op & 0x0F00) >> 0x8);

	DEBUG_PRINT("LD up to V%x into memory starting at I: %x", finalRegisterIndex, context->I);

	for (int x = 0; x <= finalRegisterIndex; x++)
	{
		context->memoryMap[context->I + x] = context->V[x];
	}
}

void handle_F65(uint16_t op, Chip8Context* context)
{
	uint8_t finalRegisterIndex = ((op & 0x0F00) >> 0x8);

	DEBUG_PRINT("LD into registers upto V%x starting at I: %x", finalRegisterIndex, context->I);

	for (int x = 0; x <= finalRegisterIndex; x++)
	{
		context->V[x] = context->memoryMap[context->I + x];
	}
}
