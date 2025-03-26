#pragma once

#include "Chip8Context.h"

typedef struct
{
	int x;
	int y;
} SpritePosition;

void draw(uint8_t* sprite, size_t size_of_sprite, SpritePosition sprite_position, Chip8Context* context);