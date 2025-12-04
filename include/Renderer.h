#pragma once

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

#include "config.h"

class Scene;

bool InitWindow(SDL_Window*& window, SDL_GLContext& glContext);
unsigned int LoadTexture(const char* path);
void DrawTriangle();
void DestroyWindow(SDL_Window* window, SDL_GLContext glContext);
void Update(const Scene& scene);

