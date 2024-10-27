#include "chip8.h"

unsigned char font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Just an empty constructor
Chip8::Chip8() {}

// Initialization
void Chip8::init()
{
    std::cout << "Initializing..." << std::endl;

    // point program counter to where the ROM is loaded
    pc = 0x200;

    int fontIndex = 0;
    for (int i = 0x50; i <= 0x09F; i++) {
        //// printf("LOADED FONT CHAR %d INTO MEMORY ADDRESS: %d\n", font[fontIndex], i);
        memory[i] = font[fontIndex];
        fontIndex++;
    }

    // printf("Loaded %d fonts to addresses 0x050 -> 0x09F\n", fontIndex + 1);

    // seed random
    srand(time(NULL));

    // printf("Done initializing\n");
}

bool Chip8::loadROM(const char* fName) {
    // printf("Beginning of load procedure\n");

    init();

    printf("Attempting to load ROM: %s\n", fName);

    FILE* file;

    errno_t err = fopen_s(&file, fName, "rb");
    if (err) {
        fprintf(stderr, "Error opening file\n");
        return false;
    }

    // Get file size
    uintmax_t fileSize = std::filesystem::file_size(fName);
    // printf("File size: %llu bytes\n", fileSize);

    // Define buffer
    char* buffer = (char*)malloc(sizeof(char) * fileSize);

    if (buffer == NULL) {
        fprintf(stderr, "Attempted to allocate buffer, but buffer pointer is null.");
        exit(1);
    }
    // Copy
    fread(buffer, sizeof(char), fileSize, file);

    if (fileSize > (4096 - 512)) {
        fprintf(stderr, "ROM too large to load into memory\n");
        return false;
    } else {
        for (int i = 0; i < fileSize; i++) {
            memory[i + 0x200] = (uint8_t)buffer[i];
        }
    }

    fclose(file);
    free(buffer);

    printf("Successfully loaded ROM\n");



    // printf("End of load procedure\n");

    return true;
}

void Chip8::cycle() {
    // printf("\n----- Start of cycle -----\n");

    // calculate opcode by merging two successive bytes together
    // shifting left byte by 8 bits gives room for second byte
    // bitwise OR to add second byte to form 16 bit (2-byte) opcode
    // printf("PC = %d\n", pc);
    uint16_t op = memory[pc] << 8 | memory[pc + 1];
    uint8_t X = (op & 0x0F00) >> 8;
    uint8_t Y = (op & 0x00F0) >> 4;
    uint8_t N = op & 0x000F;
    uint8_t NN = op & 0x00FF;
    uint16_t NNN = op & 0x0FFF;
    pc += 2;

    // printf("op = 0x%X\n", op);
    // printf("instruction = 0x%X\n", op & 0xF000);

    switch (op & 0xF000) {
        case 0x0000:
            // Some opcodes start with a 0x0 (0b0000) instruction
            //// printf("Special 0x0000 opcode\n");
            switch (NNN) {
                case 0x00E0:
                    // clear screen
                    // printf("Clear screen\n");
                    for (int i = 0; i < 64 * 32; i++) {
                        gfx[i] = 0;
                    }
                    drawFlag = true;
                    break;
                case 0x00EE:
                    // pop stack
                    // printf("Return from subroutine (pop)\n");
                    pc = stack[sp - 1];
                    sp--;
                    break;
                default:
                    printf("Unknown opcode.\n");
            }
            break;
        case 0x1000:
            // Jump
            // printf("Jump opcode\n");
            pc = NNN;
            break;
        case 0x2000:
            // call subroutine w/ stack memory
            stack[sp++] = pc;
            pc = NNN;
            break;
        case 0x3000:
            // skip if VX == NN
            // printf("Skip if condition VX == NN\n");
            if (V[X] == NN) {
                pc += 2;
            }
            break;
        case 0x4000:
            // printf("Skip if condition VX != NN\n");
            if (V[X] != NN) {
                pc += 2;
            }
            break;
        case 0x5000:
            // printf("Skip if condition VX == VY\n");
            if (V[X] == V[Y]) {
                pc += 2;
            }
            break;
        case 0x6000:
            // set register vx
            // printf("Set V%d to 0x%X\n", X, NN);
            V[X] = NN;
            break;
        case 0x7000:
            // add value to register vx
            // printf("Add 0x%X to V%d\n", NN, X);
            V[X] += NN;
            break;
        case 0x8000:
            // arithmetic/logic
            // printf("Arithmetic operation\n");
            switch (N) {
                case 0x0:
                    // set
                    // printf("Set\n");
                    V[X] = V[Y];
                    break;
                case 0x1:
                    // binary OR
                    // printf("Binary OR\n");
                    V[X] |= V[Y];
                    break;
                case 0x2:
                    // binary AND
                    // printf("Binary AND\n");
                    V[X] &= V[Y];
                    break;
                case 0x3:
                    // binary XOR
                    // printf("Binary XOR\n");
                    V[X] ^= V[Y];
                    break;
                case 0x4:
                    // add
                    // printf("Add\n");
                    if (V[X] + V[Y] > 255) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] += V[Y];
                    break;
                case 0x5:
                    // subtraction: VX - VY
                    // printf("Subtract VX - VY\n");
                    if (V[X] >= V[Y]) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] -= V[Y];
                    break;
                case 0x6:
                    // shift VX 1 bit right
                    // printf("Bit shift right\n");
                    V[0xF] = V[X] & 1;
                    V[X] >>= 1;
                    break;
                case 0x7:
                    // subtraction: VY - VX
                    // printf("Subtract VY - VX\n");
                    if (V[Y] >= V[X]) {
                        V[0xF] = 1;
                    }
                    else {
                        V[0xF] = 0;
                    }
                    V[X] = V[Y] - V[X];
                    break;
                case 0xE:
                    // shift VX 1 bit left
                    // printf("Bit shift left\n");
                    V[0xF] = V[X] >> 7;
                    V[X] <<= 1;
                    break;
                default:
                    printf("Unknown arithmetic operation\n");
            }
            break;
        case 0x9000:
            // printf("Skip if condition VX != VY\n");
            if (V[X] != V[Y]) {
                pc += 2;
            }
            break;
        case 0xA000:
            // set index register I
            // printf("Set index register I to 0x%X\n", NNN);
            I = NNN;
            break;
        case 0xB000:
            // Jump with offset
            // Using BXNN late gen quirk
            pc = NNN + V[0];
            break;
        case 0xC000:
            // Random
            V[X] = (rand() % (0xFF + 1)) & NN;
            break;
        case 0xD000:
            {
                // display/draw
                // printf("Draw\n");

                uint8_t pixel;

                uint8_t xcoord = V[X] % 64;
                uint8_t ycoord = V[Y] % 32;

                // printf("X = %d\n", xcoord);
                // printf("Y = %d\n", ycoord);

                // reset register VF flag (collision detection flag)
                V[0xF] = 0;

                if (N > 15) {
                    exit(1);
                    N = 15;
                }

                for (int row = 0; row < N; row++) {
                    pixel = memory[I + row];
                    for (int col = 0; col < 8; col++) {
                        if ((pixel & (0x80 >> col)) != 0) {
                            if (gfx[((xcoord + col) % 64) + ((ycoord + row) % 32) * 64] == 1) {
                                V[0xF] = 1;
                            }
                            gfx[((xcoord + col) % 64) + ((ycoord + row) % 32) * 64] ^= 1;
                        }
                    }
                }

                drawFlag = true;
                break;
            }
        case 0xE000:
            switch (NN) {
                case 0x009E:
                    // if key pressed skip instruction
                    if (key[V[X]] == 1) {
                        pc += 2;
                    }
                    break;
                case 0x00A1:
                    // if key NOT pressed skip instruction
                    if (key[V[X]] != 1) {
                        pc += 2;
                    }
                    break;
            }
            break;
        case 0xF000:
            switch (NN) {
                case 0x0007:
                    // set VX to delayTimer value
                    V[X] = delayTimer;
                    break;
                case 0x0015:
                    // Set delayTimer to VX value
                    delayTimer = V[X];
                    break;
                case 0x0018:
                    // set soundTimer to VY value
                    soundTimer = V[Y];
                    break;
                case 0x001E:
                    // add VX to I
                    // set VF if overflow
                    if (I + V[X] > 0xFFF) {
                        V[0xF] = 1;
                    }
                    I += V[X];
                    break;
                case 0x000A:
                {
                    // block execution until a key is pressed
                    bool keyPressed = false;
                    for (int i = 0; i < 16; i++) {
                        if (key[i] == 1) {
                            V[X] = i;
                            keyPressed = true;
                            break;
                        }
                    }
                    if (!keyPressed) {
                        pc -= 2;
                    }
                    break;
                }
                case 0x0029:
                    // set I to address of hex character in VX
                    I = V[X] * 5;
                    break;
                case 0x0033:
                    // binary-coded decimal conversion
                    memory[I] = V[X] / 100;
                    memory[I + 1] = (V[X] / 10) % 10;
                    memory[I + 2] = V[X] % 10;
                    break;
                case 0x0055:
                    // store values from registers into memory
                    for (int i = 0; i <= X; i++) {
                        memory[I + i] = V[i];
                    }

                    // original COSMAC behavior
                    //I = I + X + 1;
                    break;
                case 0x0065:
                    // load values from memory into registers
                    for (int i = 0; i <= X; i++) {
                        V[i] = memory[I + i];
                    }

                    // original COSMAC behavior
                    //I = I + X + 1;
                    break;
            }
            break;
        default:
            printf("Unknown opcode: 0x%X\n", op);
    }
    
    if (delayTimer > 0) {
        delayTimer--;
    }

    if (soundTimer > 0) {
        soundTimer--;
    }

    // printf("----- End of cycle -----\n");
}
