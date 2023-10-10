#include <chrono>
#include <thread>
#include "SDL.h"


#include "chip8.h"

using namespace std;

// Keypad keymap
uint8_t keymap[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3,
        SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c,
        SDLK_4, SDLK_r, SDLK_f, SDLK_v
};

int main(int argc, char **argv) {

    // Command usage
    if (argc != 2) {
        fprintf(stderr, "Usage: Chip8 <rom>\n");
        return 1;
    }

    Chip8 chip8; // Initialise Chip8
    int w = 1024; // Height of window
    int h = 512; // Width of window
    SDL_Window *window = nullptr;

    // Initialize
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL was not able to initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    // Create the window
    window = SDL_CreateWindow(
            "CHIP-8 Emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            w, h, SDL_WINDOW_SHOWN
    );
    if (window == nullptr){
        fprintf(stderr, "Window was not able to be created! SDL_Error: %s\n", SDL_GetError());
        exit(2);
    }

    // Create the sdlRenderer
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(window, -1, 0);

    if (sdlRenderer == nullptr) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        exit(3);
    }

    SDL_RenderSetLogicalSize(sdlRenderer, w, h);

    // Create the texture
    SDL_Texture *sdlTexture = SDL_CreateTexture(sdlRenderer,
                                                SDL_PIXELFORMAT_ARGB8888,
                                                SDL_TEXTUREACCESS_STREAMING,
                                                64, 32);

    if (sdlTexture == nullptr) {
        fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(sdlRenderer); // Clean up the renderer
        exit(4);
    }

    // Buffer for pixels
    uint32_t pixels[2048];

    // Attempt to load the ROM
    bool loadROMSuccess = false;
    while (!loadROMSuccess) {
        // Attempt to load the ROM
        if (chip8.loadROM(argv[1])) {
            loadROMSuccess = true;
        } else {
            return 2; // Return an error code on failed ROM load
        }
    }

    // Emulation loop
    while (true) {
        chip8.emulate_cycle();

        // Process SDL events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }

            // Process keydown
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        exit(0);
                    case SDLK_F1:
                        // Reload the ROM
                        loadROMSuccess = false;
                        break;
                    default:
                        for (int i = 0; i < 16; ++i) {
                            if (e.key.keysym.sym == keymap[i]) {
                                chip8.keypad[i] = 1;
                            }
                        }
                        break;
                }
            }
            // Process keyup events
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.keypad[i] = 0;
                    }
                }
            }
        }

        // If draw occurs -- redraw SDL screen
        if (chip8.drawFlag) {
            chip8.drawFlag = false;

            // Store pixels in temporary buffer
            for (int i = 0; i < 2048; ++i) {
                uint8_t pixel = chip8.display[i];
                pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
            }

            // Update the SDL texture
            SDL_UpdateTexture(sdlTexture, nullptr, pixels, 64 * sizeof(Uint32));

            // Clear the SDL renderer
            SDL_RenderClear(sdlRenderer);

            // Copy the texture to the renderer
            SDL_RenderCopy(sdlRenderer, sdlTexture, nullptr, nullptr);

            // Present the renderer
            SDL_RenderPresent(sdlRenderer);
        }

        // Slow down the speed of emulation
        std::this_thread::sleep_for(std::chrono::microseconds(1200));

    }
}