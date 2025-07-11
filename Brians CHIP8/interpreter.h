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

// 2nnn - CALL addr: call subroutine at addr nnn
void handle_2(uint16_t op, Chip8Context* context);

// 3xnn - SE Vx, byte: skip next instruction if Vx = kk
void handle_3(uint16_t op, Chip8Context* context);

// 4xkk - SNE Vx, byte: skip next instruction if Vx != kk
void handle_4(uint16_t op, Chip8Context* context);

// 5xy0 - SE Vx, Vy: skip next instruction if Vx = Vy
void handle_5(uint16_t op, Chip8Context* context);

// 7xkk - ADD Vx, byte: Vx += kk
void handle_7(uint16_t op, Chip8Context* context);

// 9xy0 - SNE Vx, Vy: Skip next instruction if Vx != Vy
void handle_9(uint16_t op, Chip8Context* context);

// Annn - LD I, addr - The value of register I is set to nnn
void handle_A(uint16_t op, Chip8Context* context);

// Dxyn - DRW Vx, Vy, n - display n-bytes sprite starting at memory location I at (Vx, Vy), set VF = collision
void handle_D(uint16_t op, Chip8Context* context);

// Fx0A - LD Vx, K: block until a keypress, store in Vx
void handle_F0A(uint16_t op, Chip8Context* context);

// Fx1E - ADD I, Vx: Set I = I + Vx
void handle_F1E(uint16_t op, Chip8Context* context);

// Fx33 - LD B, Vx: store BCD representation of Vx in memory locations I, I+1, and I. The interpreter takes the decimal value of Vx, and places hundreds, tens, ones into location I starting at I
void handle_F33(uint16_t op, Chip8Context* context);

// Fx55 - LD [I], Vx: store V0 through Vx into memory starting at I
void handle_F55(uint16_t op, Chip8Context* context);

// Fx65 - LD Vx, [I]: Load V0 through Vx from incrementing from location I
void handle_F65(uint16_t op, Chip8Context* context);

InstructionHandler* handler_table;