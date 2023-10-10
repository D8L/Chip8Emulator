#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "chip8.h"
#include <vector>

Chip8::Chip8() = default; // Empty
Chip8::~Chip8() = default; // Empty

unsigned char fontROM[80] = {
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

void Chip8::initialize() {

    // Initialization of variables

    indexRegister = 0;  // Reset indexRegister (I)
    opcode = 0;        // Reset op code
    sp = 0;        // Reset stack pointer
    pc = 0x200;    // Set program counter to 0x200
    delayTimer = 0;     // Reset delayTimer
    soundTimer = 0;
    srand(time(nullptr)); // Set RNG

    // Clear the stack, keypad, and varRegister
    for (int i = 0; i < 16; ++i) {
        stack[i] = 0;
        keypad[i] = 0;
        varRegister[i] = 0;
    }

    // Clear memory
    for (unsigned char &i: memory) {
        i = 0;
    }

    // Load font set into the memory
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontROM[i];
    }

    // Clear the display
    for (unsigned char &i: display) {
        i = 0;
    }
}

// Emulate one cycle
void Chip8::emulate_cycle() {

    // Fetch op code
    opcode = memory[pc] << 8 | memory[pc + 1];   // Op code is two bytes

    uint8_t X = (opcode & 0x0F00) >> 8; // lower 4 bits of the high byte (the second nibble)
    uint8_t Y = (opcode & 0x00F0) >> 4;  // upper 4 bits of the low byte (the third nibble)
    uint8_t N = (opcode & 0x000F); // lowest 4 bits (the fourth nibble)
    uint8_t NN = (opcode & 0x00FF); // lowest 8 bits (the third and fourth nibbles)
    uint16_t NNN = (opcode & 0X0FFF); // lowest 12 bits (the second, third, and fourth nibbles)


    switch (opcode & 0xF000) {

        // 00E_
        case 0x0000:

            switch (N) {
                // 00E0 - Clear video memory
                case 0x0000:
                    for (unsigned char &i: display) {
                        i = 0;
                    }
                    drawFlag = true;
                    pc += 2;
                    break;

                    // 00EE - Return from subroutine
                case 0x000E:
                    pc = stack[--sp];
                    pc += 2;
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            }
            break;

            // 1NNN - Jump to address NNN
        case 0x1000:
            pc = NNN;
            break;

            // 2NNN - Call CHIP-8 subroutine at NNN
        case 0x2000:
            stack[sp] = pc;
            ++sp;
            pc = NNN;
            break;

            // 3XNN - Skip next instruction if VX == NN
        case 0x3000:
            if (varRegister[X] == (NN)) {
                pc += 4;
            } else {
                pc += 2; }
            break;

            // 4XNN - Skip next instruction if VX != NN
        case 0x4000:
            if (varRegister[X] != (NN)) {
                pc += 4;
            } else {
                pc += 2; }
            break;

            // 5XY0 - Skip next instruction if VX == VY
        case 0x5000:
            if (varRegister[X] == varRegister[Y]) {
                pc += 4;
            } else {
                pc += 2; }
            break;

            // 6XNN - VX = NN
        case 0x6000:
            varRegister[X] = NN;
            pc += 2;
            break;

            // 7XNN - VX = VX + NN
        case 0x7000:
            varRegister[X] += NN;
            pc += 2;
            break;

            // 8XY_
        case 0x8000:
            switch (N) {

                // 8XY0 - VX = VY
                case 0x0000:
                    varRegister[X] = varRegister[Y];
                    pc += 2;
                    break;

                    // 8XY1 - VX = VX OR VY
                case 0x0001:
                    varRegister[X] |= varRegister[Y];
                    pc += 2;
                    break;

                    // 8XY2 - VX = VX AND VY
                case 0x0002:
                    varRegister[X] &= varRegister[Y];
                    pc += 2;
                    break;

                    // 8XY3 - VX = VX XOR VY
                case 0x0003:
                    varRegister[X] ^= varRegister[Y];
                    pc += 2;
                    break;

                    // 8XY4 - VX = VX + VY; VF = 1 if overflow else 0
                case 0x0004:
                    varRegister[X] += varRegister[Y];
                    if (varRegister[Y] > (0xFF - varRegister[X])) {
                        varRegister[0xF] = 1;
                         } else {
                        varRegister[0xF] = 0; }
                    pc += 2;
                    break;

                    // 8XY5 - VX = VX - VY; VF = 1 if not borrow else 0
                case 0x0005:
                    if (varRegister[X] > varRegister[Y]) {
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0; }
                    varRegister[X] -= varRegister[Y];
                    pc += 2;
                    break;

                    // 0x8XY6 - VF = LSB(VX); VX = VX » 1
                case 0x0006:
                    varRegister[0xF] = varRegister[X] & 0x1;
                    varRegister[X] >>= 1;
                    pc += 2;
                    break;

                    // 0x8XY7: VX = VY - VX; VF = 1 if not borrow else 0
                case 0x0007:
                    if (varRegister[X] > varRegister[Y]) {
                        varRegister[0xF] = 0;
                    } else {
                        varRegister[0xF] = 1; }
                    varRegister[X] = varRegister[Y] - varRegister[X];
                    pc += 2;
                    break;

                    // 0x8XYE: VF = MSB(VX); VX = VX « 1
                case 0x000E:
                    varRegister[0xF] = varRegister[X] >> 7;
                    varRegister[X] <<= 1;
                    pc += 2;
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            }
            break;

            // 9XY0 - Skip next instruction if VX != VY
        case 0x9000:
            if (varRegister[X] != varRegister[Y]) {
                pc += 4;
            } else {
                pc += 2; }
            break;

            // ANNN - I = NNN
        case 0xA000:
            indexRegister = NNN;
            pc += 2;
            break;

            // BNNN - Jump to address NNN + V0
        case 0xB000:
            pc = (NNN) + varRegister[0];
            break;

            // CXNN - VX = RND() AND NN
        case 0xC000:
            varRegister[X] = (rand() % (0xFF + 1)) & (NN);
            pc += 2;
            break;

            // DXYN: Draw 8xN sprite at I to VX, VY; VF = 1 if collision else 0
        case 0xD000: {
            uint8_t Vx = varRegister[X];
            uint8_t Vy = varRegister[Y];
            uint8_t pixel;

            varRegister[0xF] = 0;
            for (int dY = 0; dY < N; dY++) {
                pixel = memory[dY + indexRegister];
                for (int dX = 0; dX < 8; dX++) {
                    if (0 != (pixel & (0x80 >> dX))) {
                        if (1 == display[(Vx + dX + ((Vy + dY) * 64))]) {
                            varRegister[0xF] = 1;
                        }
                        display[Vx + dX + ((Vy + dY) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
            break;

            // EX__
        case 0xE000:

            switch (NN) {
                // EX9E - Skip next instruction if key(VX) is pressed
                case 0x009E:
                    if (keypad[varRegister[X]] != 0) {
                        pc += 4;
                    } else {
                        pc += 2; }
                    break;

                    // EXA1 - Skip next instruction if key(VX) is not pressed
                case 0x00A1:
                    if (keypad[varRegister[X]] == 0) {
                        pc += 4;
                    } else {
                        pc += 2; }
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            }
            break;

            // FX__
        case 0xF000:
            switch (NN) {
                // FX07 - VX = delayTimer
                case 0x0007:
                    varRegister[X] = delayTimer;
                    pc += 2;
                    break;

                    // FX0A - Wait for key press, store key pressed in VX
                case 0x000A: {
                    bool keyPress = false;

                    for (int i = 0; i < 16; ++i) {
                        if (keypad[i] != 0) {
                            varRegister[X] = i;
                            keyPress = true;
                        }
                    }

                    // If there is no key press — try again
                    if (!keyPress)
                        return;

                    pc += 2;
                }
                    break;

                    // FX15 - delayTimer = VX
                case 0x0015:
                    delayTimer = varRegister[X];
                    pc += 2;
                    break;

                    // FX18 - soundTimer = VX
                case 0x0018:
                    soundTimer = varRegister[X];
                    pc += 2;
                    break;

                    // FX1E - I = I + VX; VF = 1 if I > 0xFFF else 0
                case 0x001E:
                    if (indexRegister + varRegister[X] > 0xFFF) {
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0; }
                    indexRegister += varRegister[X];
                    pc += 2;
                    break;

                    // FX29 - I = address of 4x5 font character in VX (0..F)
                case 0x0029:
                    indexRegister = varRegister[X] * 0x5;
                    pc += 2;
                    break;

                    // FX33 - Store BCD representation of VX at I (100), I+1 (10), and I+2 (1); I remains unchanged
                case 0x0033:
                    memory[indexRegister] = varRegister[X] / 100;
                    memory[indexRegister + 1] = (varRegister[X] / 10) % 10;
                    memory[indexRegister + 2] = varRegister[X] % 10;
                    pc += 2;
                    break;

                    // FX55 - Store V0..VX (inclusive) to memory starting at I; I remains unchanged
                case 0x0055:
                    for (int i = 0; i <= X; ++i)
                        memory[indexRegister + i] = varRegister[i];
                    indexRegister += X + 1;
                    pc += 2;
                    break;

                case 0x0065:
                    for (int i = 0; i <= X; ++i)
                        varRegister[i] = memory[indexRegister + i];
                    indexRegister += X + 1;
                    pc += 2;
                    break;

                default:
                    fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
            }
            break;

        default:
            fprintf(stderr, "Unknown opcode: 0x%X\n", opcode);
    }

    // Update delayTimer
    if (delayTimer > 0) {
        --delayTimer;
    }
}

// Initialize and load the ROM
bool Chip8::loadROM(const char *file_path) {
    initialize();

    // Check size of the ROM file
    FILE *rom = fopen(file_path, "rb");
    if (nullptr == rom) {
        fputs("File error", stderr);
        return false;
    }

    // Allocate memory to store the ROM file
    fseek(rom, 0, SEEK_END);
    long romSize = ftell(rom);
    rewind(rom);

    // Copy the ROM into the buffer
    char *rom_buffer = (char *) malloc(sizeof(char) * romSize);
    if (nullptr == rom_buffer) {
        fprintf(stderr, "Failed to allocate memory for ROM\n");
        fclose(rom);
        return false;
    }

    // Copy the buffer to the Chip8-Emulator memory
    size_t result = fread(rom_buffer, sizeof(char), (size_t) romSize, rom);
    if (result != romSize) {
        fprintf(stderr, "Failed to read ROM\n");
        fclose(rom);
        free(rom_buffer);
        return false;
    }

    if ((4096 - 512) > romSize) {
        for (int i = 0; i < romSize; ++i) {
            memory[i + 512] = (uint8_t)rom_buffer[i];
        }
    } else {
        fprintf(stderr, "ROM too large to fit in memory\n");
        fclose(rom);
        free(rom_buffer);
        return false;
    }

    fclose(rom);
    free(rom_buffer);

    return true;
}