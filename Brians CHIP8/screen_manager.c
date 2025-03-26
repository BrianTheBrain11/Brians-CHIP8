#include "screen_manager.h"

void draw(uint8_t* sprite, size_t size_of_sprite, SpritePosition sprite_position, Chip8Context* context)
{

	SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, 255);

	context->V[0xF] = 0; // clear VF

	for (int i = 0; i < size_of_sprite; i++) // size of sprite in bytes, row by row
	{
		int byte_index = sprite_position.x / 8; // gets us the first byte we need to read off
		int bit_index = sprite_position.x % 8; // gets us the specific bits we need from this byte

		uint8_t first_byte = context->video_memory_map[sprite_position.y][byte_index];
		//uint8_t first_byte = context->video_memory_map[0][0];
		uint8_t second_byte = context->video_memory_map[sprite_position.y][byte_index + 1];

		uint8_t video_mem_row = (first_byte << bit_index) | (second_byte >> (8 - bit_index)); // we need to OR the first and second bytes shifted to get one complete byte

		uint8_t sprite_row = sprite[i];

		// we XOR to draw to screen
		uint8_t new_row = sprite_row ^ video_mem_row;

		if (new_row != sprite_row) // if any bits were cleared
		{
			context->V[0xF] = 1; // Set VF to 1
		}

		// now we actually draw this row
		for (int pixel = 0; pixel < 8; pixel++)
		{
			if (((new_row >> pixel) & 0x01) > 0) // shift to get the correct pixel in the LSbit position, then mask to get the bit
			{
				printf("here supposedly");
				SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(context->renderer, sprite_position.x + i, sprite_position.y);
			}
			else
			{
				printf("over here");
				SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
				SDL_RenderDrawPoint(context->renderer, sprite_position.x + i, sprite_position.y);
			}
		}
	}
}
