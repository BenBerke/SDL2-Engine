#pragma once
#include <SDL2/SDL.h>

class GameTime {
public:
    inline static float deltaTime = 0.0f;     
    inline static float time = 0.0f;          

    static void Update() {
        Uint64 now = SDL_GetPerformanceCounter();
        static Uint64 last = now;

        Uint64 diff = now - last;
        last = now;

        deltaTime = (float)(diff / (double)SDL_GetPerformanceFrequency());
        time += deltaTime;
    }
};
