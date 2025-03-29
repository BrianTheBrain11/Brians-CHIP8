#include "screen_manager.h"

#include <stdio.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

//#define GRAPHICS_DEBUG

#ifdef GRAPHICS_DEBUG
#ifdef _DEBUG
#define GRAPHICS_DEBUG_PRINT printf
#endif
#else
#define GRAPHICS_DEBUG_PRINT //
#endif

void clear_display(Chip8Context* context)
{
	SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);

	for (int y = 0; y < 32; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 0, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 1, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 2, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 3, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 4, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 5, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 6, y);
			SDL_RenderDrawPoint(context->renderer, (8 * x) + 7, y);

			context->video_memory_map[y][x] = (uint8_t)0x00;
		}
	}

	SDL_RenderPresent(context->renderer);

}

void draw(uint8_t* sprite, size_t size_of_sprite, SpritePosition sprite_position, Chip8Context* context)
{

	context->V[0xF] = 0; // clear VF

	for (int i = 0; i < size_of_sprite; i++) // size of sprite in bytes, row by row
	{
		int byte_index = sprite_position.x / 8; // gets us the first byte we need to read off
		int bit_index = sprite_position.x % 8; // gets us the specific bits we need from this byte

		int evaluated_row_y_position = (sprite_position.y + i) % 32;

		uint8_t* first_byte = &context->video_memory_map[evaluated_row_y_position][byte_index];
		uint8_t* second_byte = &context->video_memory_map[evaluated_row_y_position][(byte_index + 1) % 8];

		uint8_t video_mem_row = (*first_byte << bit_index) | (*second_byte >> (8 - bit_index)); // we need to OR the first and second bytes shifted to get one complete byte

		uint8_t sprite_row = sprite[i];


		// we XOR to draw to screen
		uint8_t new_row = sprite_row ^ video_mem_row;

		if (new_row != sprite_row) // if any bits were cleared
		{
			context->V[0xF] = 1; // Set VF to 1
		}



		GRAPHICS_DEBUG_PRINT("row number: %d\n", i);
		GRAPHICS_DEBUG_PRINT("sprite_row: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(sprite_row));
		GRAPHICS_DEBUG_PRINT("video_mem_row: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(video_mem_row));
		GRAPHICS_DEBUG_PRINT("new_row: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(new_row));


		uint8_t first_mask = (0xFF << (8 - bit_index));
		*first_byte &= first_mask; // leaves us with all 0s where we want to mask
		*first_byte |= (new_row >> bit_index);
		uint8_t second_mask = ~((0xFFF) << (8 - bit_index)); // leaves us with all 0's where we want to mask
		*second_byte &= second_mask; //gives us all 0's where we want to now write to
		*second_byte |= (new_row << (8 - bit_index));

		// clear the relevant bits in the target memory
		/*uint8_t mask_first = ((1 << (8 - bit_index)) - 1);      // bits affected in first_byte
		uint8_t mask_second = 0xFF >> (8 - bit_index);          // bits affected in second_byte

		*first_byte &= ~mask_first;
		*second_byte &= ~mask_second;

		// write the new value into the cleared areas
		*first_byte |= new_row >> bit_index;
		*second_byte |= new_row << (8 - bit_index);*/

		GRAPHICS_DEBUG_PRINT("new video mem row i: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(*first_byte));
		GRAPHICS_DEBUG_PRINT("new video mem row i + 1: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(*second_byte));

		// now we actually draw this row
		for (int pixel = 0; pixel < 8; pixel++)
		{
			if (((new_row >> (7 - pixel)) & 0x01) > 0) // shift to get the correct pixel in the LSbit position, then mask to get the bit
			{
				SDL_SetRenderDrawColor(context->renderer, 255, 0, 0, 255);
				SDL_RenderDrawPoint(context->renderer, (sprite_position.x + pixel) % 64, evaluated_row_y_position);
			}
			else
			{
				SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 0);
				SDL_RenderDrawPoint(context->renderer, (sprite_position.x + pixel) % 64, evaluated_row_y_position);
			}
		}

		//SDL_RenderPresent(context->renderer);
	}
}