#pragma once

#include <stdint.h>

#define FULL_DEBUG
#ifdef FULL_DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT //
#endif

#include "Chip8Context.h"

typedef void (*InstructionHandler)(uint16_t op);

// 1nnn - JP addr - jump to location nnn
void handle_1(uint16_t op, Chip8Context* context);

// Annn - LD I, addr - The value of register I is set to nnn
void handle_A(uint16_t op, Chip8Context* context);

// Dxyn - DRW Vx, Vy, n - display n-bytes sprite starting at memory location I at (Vx, Vy), set VF = collision
void handle_D(uint16_t op, Chip8Context* context);

InstructionHandler* handler_table;