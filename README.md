
# SegoUI

SegoUI is a header-only user interface (UI) library for SDL3 and GLM. Its architecture uses a scene-tree system inspired by the node-based logic of the Godot Engine.


## Key Features

- Header-Only: Easy integration by including just one header file.

- Scene-Tree Architecture: Parent-Child system with automatic (recursive) global position calculation.

- Auto-ID: Uses the FNV-1a string hashing algorithm for node identification without manual input.

- GLM Integration: Leverages the glm::vec2 and glm::vec4 data types for precise position and color calculations.

- Container System: VBoxContainer and HBoxContainer are available for automatic layout.


## System Requirements

- [SDL3](https://github.com/libsdl-org/SDL)
- [SDL3_image](https://github.com/libsdl-org/SDL_image)
- [SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)
- [GLM](https://github.com/g-truc/glm)

## Installation

Copy the SegoUI.hpp file into your project directory and ensure the SDL3 and GLM libraries are linked in your build system.
## Usage/Examples

```c++
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include "../SegoUI/SegoUI.hpp"
#include <cstdint>
#include <iostream>


int main(){
    std::cerr << "Mencoba SDL_Init..." << std::endl;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        SDL_Log("Error: %s", SDL_GetError());
        return -1;
    }

    if (!UIContext::get().init_font("assets/AgaveNerdFont-Regular.ttf", 18)) {
        SDL_Log("Failed load font...");
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("TEST",1280, 720, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "Failed window: " << SDL_GetError() << std::endl;
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
    if (!renderer) {
        std::cerr << "Failed window: " << SDL_GetError() << std::endl;
        return -1;
    }

    UIPanel* editor_root = new UIPanel("RootPanel", {0,0}, {1280, 720});
    UIButton* btn_save = new UIButton("ButtonSave", {20, 20}, {150, 40});

    editor_root->add_child(btn_save);


    bool running = true;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
        }

        float mx,my;
        uint32_t ms = SDL_GetMouseState(&mx, &my);
        bool mDown = (ms & SDL_BUTTON_LMASK);

        UIContext::get().hotID = 0;
        editor_root->update({mx,my}, mDown);


        if (btn_save->wasClicked) {
            SDL_Log("Tombol Save Ditekan!");
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        editor_root->draw(renderer);

        SDL_RenderPresent(renderer);


    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

```

