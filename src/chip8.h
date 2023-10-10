#ifndef CHIP_8_H
#define CHIP_8_H
#include <cstdint>

class Chip8 {

public:
    uint8_t  keypad[16]{};                   // Keypad
    uint8_t  display[64 * 32]{};             // Display
    bool drawFlag{};                         // Flag for drawing
    Chip8();
    ~Chip8();
    void emulate_cycle();               // Single cycle emulation
    bool loadROM(const char *file_path);   // Load ROM

private:
    uint16_t stack[16]{};                 // Stack
    uint8_t  varRegister[16]{};                     // Registers
    uint8_t memory[4096]{};               // Memory
    uint16_t opcode{};                    // Opcode
    uint16_t sp{};                        // Stack Pointer
    uint16_t pc{};                        // Program Counter
    uint16_t indexRegister{};             // Index register
    uint8_t delayTimer{};                 // Delay timer
    uint8_t soundTimer{};                 // Delay timer
    void initialize();

};

#endif // CHIP_8_H