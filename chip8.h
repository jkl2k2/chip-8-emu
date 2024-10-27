#pragma once

#include <iostream>
#include <cstdint>
#include <filesystem>
#include <random>
#include <time.h>

using std::uint16_t;
using std::uint8_t;
using std::uintmax_t;

class Chip8 {
private:
	// 4096 byte RAM
	uint8_t memory[4096] = {};

	// 8 bit program counter
	// points at the current instruction in memory
	uint16_t pc{};

	// 16 bit index register
	// used to point at locations in memory
	uint16_t I{};


	// 16 bit stack with 16 capacity
	// used to call subroutines/functions and return from them
	uint16_t stack[16] = {};
	uint16_t sp{};

	// 8 bit delay timer
	// decremented at a rate of 60 Hz (60 times per second) until it reaches 0
	uint8_t delayTimer{};

	// 8 bit sound timer
	// functions like the delay timer, but which also gives off a beeping sound as long as it’s not 0
	uint8_t soundTimer{};

	// 16 8-bit variable registers V0-VF in hex
	// VF is also used as a flag register
	// many instructions will set it to either 1 or 0 based on some rule
	// for example using it as a carry flag
	uint8_t V[16] = {};

	void init();
public:
	uint8_t gfx[64 * 32] = {};
	uint8_t key[16] = {};

	Chip8();

	bool drawFlag = false;

	bool loadROM(const char* fName);
	void cycle();
};