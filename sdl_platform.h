#pragma once

#include <cstdint>

#include "chip8.h"

using std::uint16_t;
using std::uint8_t;

class SDL_Platform {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* sdlTexture;
public:
	SDL_Platform(int windowWidth, int windowHeight, int logicalWidth, int logicalHeight);
	~SDL_Platform();

	void updateTexture(uint32_t* buffer);
	void input(Chip8& chip8, uint8_t* keymap);
};