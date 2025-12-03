#pragma once
#include <iostream>
#include "Components/CustomBehaviour.h"
#include "InputManager.h"

class TestBehaviour : public CustomBehaviour{
public:
float message = 5.0f;

void Update() override{
    if(InputManager::GetKeyDown(SDL_SCANCODE_F)) std::cout << message << std::endl;
}
};