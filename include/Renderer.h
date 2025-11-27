#pragma once

#ifndef SDL_MAIN_HANDLED
#define SDL_MAIN_HANDLED
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "config.h"

class Scene;

// Sets up SDL, SDL_image, the window, and the renderer.
bool InitRenderer(SDL_Window*& window, SDL_Renderer*& renderer);

// Clears the current frame with the given color.
void BeginFrame(SDL_Renderer* renderer, SDL_Color clearColor = {0, 0, 0, SDL_ALPHA_OPAQUE});

// Renders all SpriteRenderers in the provided scene.
void RenderScene(SDL_Renderer* renderer, Scene& scene, SDL_Color clearColor = {255, 255, 255, SDL_ALPHA_OPAQUE}, SDL_Point pivot = {-1, -1});

// Presents the backbuffer to the screen.
void EndFrame(SDL_Renderer* renderer);

// Cleans up SDL resources and quits SDL subsystems.
void DestroyRenderer(SDL_Window* window, SDL_Renderer* renderer);
