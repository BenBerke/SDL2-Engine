#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "GameTime.h"
#include "InputManager.h"
#include "Renderer.h"
#include "Physics.h"

#include "Scene.h"
#include "GameObject.h"

#include "Components.h"


int main()
{
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;

    if (!Renderer::InitWindow(window, glContext))
        return -1;

    Scene currentScene;

    GameObject& obj = currentScene.CreateObject<GameObject>();
    obj.AddComponent<SpriteRenderer>();
    obj.GetComponent<SpriteRenderer>()->SetTexture("Assets/Textures/container.jpg");
    obj.AddComponent<Rigidbody>();
    obj.AddComponent<BoxCollider>();
    auto t = obj.GetComponent<Transform>();

    GameObject& floor = currentScene.CreateObject<GameObject>();
    floor.GetComponent<Transform>()->position = {0, -5};
    floor.AddComponent<SpriteRenderer>();
    floor.GetComponent<SpriteRenderer>()->SetTexture("Assets/Textures/container.jpg");
    floor.AddComponent<BoxCollider>();
    floor.GetComponent<Transform>()->scale = {1, 1};


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

        if (InputManager::GetKey(SDL_SCANCODE_A)) t->position.x -= .01f;
        if (InputManager::GetKey(SDL_SCANCODE_D)) t->position.x += .01f;

        Physics::Update(currentScene);
        GameTime::Update();
        Renderer::Update(currentScene);
        SDL_GL_SwapWindow(window);
    }

    Renderer::DestroyWindow(window, glContext);
    return 0;
}