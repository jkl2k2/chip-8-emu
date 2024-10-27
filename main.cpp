#include "main.h"

const int width = 1024;
const int height = 512;
const int logicalWidth = 64;
const int logicalHeight = 32;

uint8_t keymap[16] = {
    SDLK_x,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_q,
    SDLK_w,
    SDLK_e,
    SDLK_a,
    SDLK_s,
    SDLK_d,
    SDLK_z,
    SDLK_c,
    SDLK_4,
    SDLK_r,
    SDLK_f,
    SDLK_v,
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <Path to ROM>" << std::endl;
        return 1;
    }

    Chip8 chip8 = Chip8();
    GraphicsPlatform sdl = GraphicsPlatform(width, height, logicalWidth, logicalHeight);

    uint32_t pixels[2048] = {};

    if (!chip8.loadROM(argv[1])) {
        return 2;
    }

    const std::chrono::milliseconds frameDuration(4);
    auto lastCycleTime = std::chrono::high_resolution_clock::now();

    while (true) {
        auto currentCycleTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = currentCycleTime - lastCycleTime;

        // Only process if elapsed time since last cycle meets or exceeds frame duration
        if (elapsed >= frameDuration) {
            lastCycleTime = currentCycleTime;

            sdl.input(chip8, keymap);
            chip8.cycle();

            if (chip8.drawFlag) {
                chip8.drawFlag = false;

                for (int i = 0; i < 2048; ++i) {
                    uint8_t pixel = chip8.gfx[i];
                    pixels[i] = (0x00D5D5D5 * pixel) | 0xFF353535;
                }

                sdl.updateTexture(pixels);
            }

            // Calculate remaining sleep time if needed
            auto sleepTime = frameDuration - std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
            if (sleepTime > std::chrono::milliseconds(0)) {
                std::this_thread::sleep_for(sleepTime);
            }
        }
    }

    // printf("End of main() reached.");
    return 0;
}
