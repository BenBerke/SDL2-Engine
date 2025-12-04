#pragma once

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "config.h"

class Scene;

namespace Renderer{
    bool InitWindow(SDL_Window*& window, SDL_GLContext& glContext);
    int GenerateTexture(const char* imagePath);
    void DrawTriangle();
    void DestroyWindow(SDL_Window* window, SDL_GLContext glContext);
    void Update(const Scene& scene);
}

