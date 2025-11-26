#include "InputManager.h"
#include <cstring>

const Uint8* InputManager::keyboardState = nullptr;
Uint8 InputManager::prevKeyboardState[SDL_NUM_SCANCODES] = {0};

void InputManager::Update(){
    std::memcpy(prevKeyboardState, SDL_GetKeyboardState(nullptr), SDL_NUM_SCANCODES);
    SDL_PumpEvents();
    keyboardState = SDL_GetKeyboardState(nullptr);
}

bool InputManager::GetKeyDown(SDL_Scancode key){
    return keyboardState[key] && !prevKeyboardState[key];
}

bool InputManager::GetKey(SDL_Scancode key){
    return keyboardState[key];
}

bool InputManager::GetKeyUp(SDL_Scancode key){
    return !keyboardState[key] && prevKeyboardState[key];
}