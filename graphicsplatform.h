#pragma once

#include <SDL.h>
#include <iostream>
#include <cstdint>

#include "chip8.h"

using std::uint32_t;
using std::uint16_t;
using std::uint8_t;

class GraphicsPlatform {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* sdlTexture;
public:
	GraphicsPlatform(int windowWidth, int windowHeight, int logicalWidth, int logicalHeight);
	~GraphicsPlatform();

	void updateTexture(uint32_t* buffer);
	void input(Chip8& chip8, uint8_t* keymap);
};