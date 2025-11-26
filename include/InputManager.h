#pragma once
#include <SDL2/SDL.h>

class InputManager{
public:
static void Update();
static bool GetKeyDown(SDL_Scancode key);
static bool GetKey(SDL_Scancode key);
static bool GetKeyUp(SDL_Scancode key);

private:
static const Uint8* keyboardState;
static Uint8 prevKeyboardState[SDL_NUM_SCANCODES];
};