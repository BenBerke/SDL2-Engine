#pragma once

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

struct SDL_Window;
using SDL_GLContext = void*; // SDL2 uses a void* handle for GL contexts

class Scene;

namespace Renderer{
    bool InitWindow(SDL_Window*& window, SDL_GLContext& glContext);
    int GenerateTexture(const char* imagePath);
    void DrawTriangle();
    void DestroyWindow(SDL_Window* window, SDL_GLContext glContext);
    void Update(const Scene& scene);
}
