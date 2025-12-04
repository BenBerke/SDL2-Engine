#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <glad/glad.h>

#include "GameObject.h"
#include "config.h"
#include "Scene.h"

#include "GameTime.h"
#include "Physics.h"
#include "InputManager.h"
#include "Renderer.h"

#include "core/Vector.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/SpriteRenderer.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/CustomBehaviour.h"

#include "CustomBehaviours/TestBehaviour.h"


int main()
{
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;

    if (!Renderer::InitWindow(window, glContext))
        return -1;

    Scene currentScene;

    GameObject& obj = currentScene.CreateObject<GameObject>();
    obj.AddComponent<SpriteRenderer>();
   // obj.GetComponent<SpriteRenderer>()->SetTexture("Textures/container.jpg");

    bool running = true;
    SDL_Event event;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            running = false;
        }
        InputManager::Update();

        if (InputManager::GetKey(SDL_SCANCODE_A)) {
            auto t = obj.GetComponent<Transform>();
            t->position.x -= -.01f;
        }

        Physics::Update(currentScene);
        GameTime::Update();
        Renderer::Update(currentScene);
        SDL_GL_SwapWindow(window);
    }

    Renderer::DestroyWindow(window, glContext);
    return 0;
}