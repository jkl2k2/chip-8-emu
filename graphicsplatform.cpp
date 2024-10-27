#include "graphicsplatform.h"

GraphicsPlatform::GraphicsPlatform(int windowWidth, int windowHeight, int logicalWidth, int logicalHeight) {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}

	window = SDL_CreateWindow("CHIP-8-EMU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (!window) {
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(renderer, logicalWidth, logicalHeight);

	sdlTexture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		logicalWidth, logicalHeight);
}

GraphicsPlatform::~GraphicsPlatform() {
	SDL_DestroyTexture(sdlTexture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}


void GraphicsPlatform::updateTexture(uint32_t* buffer) {
	// Update SDL texture
	SDL_UpdateTexture(sdlTexture, NULL, buffer, 64 * sizeof(Uint32));
	// Clear screen and render
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void GraphicsPlatform::input(Chip8& chip8, uint8_t* keymap) {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_QUIT) exit(0);

		// Process keydown events
		if (e.type == SDL_KEYDOWN) {
			if (e.key.keysym.sym == SDLK_ESCAPE)
				exit(0);

			for (int i = 0; i < 16; ++i) {
				if (e.key.keysym.sym == keymap[i]) {
					chip8.key[i] = 1;
				}
			}
		}
		// Process keyup events
		if (e.type == SDL_KEYUP) {
			for (int i = 0; i < 16; ++i) {
				if (e.key.keysym.sym == keymap[i]) {
					chip8.key[i] = 0;
				}
			}
		}
	}
}