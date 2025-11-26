#include <iostream>

#include "Renderer.h"

#include "GameObject.h"
#include "config.h"
#include "Scene.h"
#include "GameTime.h"
#include "Physics.h"

#include "Physics.cpp"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/SpriteRenderer.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"

#include <SDL2/SDL.h>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (!InitRenderer(window, renderer))
    {
        return 1;
    }

    Scene currentScene;

    GameObject& player = currentScene.CreateObject<GameObject>();
    player.AddComponent<SpriteRenderer>(Vector2{1,1});
    player.AddComponent<BoxCollider>(1, 1);
    player.AddComponent<Rigidbody>();

    GameObject& floor = currentScene.CreateObject<GameObject>();
    floor.GetComponent<Transform>()->position = Vector2{0, -7};
    floor.AddComponent<SpriteRenderer>(Vector2{5,1});
    floor.AddComponent<BoxCollider>(5, 1);


    GameObject& camera = currentScene.CreateObject<GameObject>();
    Camera& cameraComponent = camera.AddComponent<Camera>();

    currentScene.SetActiveCamera(cameraComponent);

    bool running = true;
    while (running)
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(nullptr);
        Rigidbody* playerRb = player.GetComponent<Rigidbody>();
        Transform* playerTransform = player.GetComponent<Transform>();
        Collider* playerCollider = player.GetComponent<Collider>();

        if (playerRb){
            if (state[SDL_SCANCODE_A]) playerRb->AddForce(Vector2{-0.2f, 0.0f});
            if (state[SDL_SCANCODE_D]) playerRb->AddForce(Vector2{0.2f, 0.0f});

            bool grounded = false;
            if (playerCollider && playerTransform){
                grounded = Physics::Collides(*playerCollider);
            }
            if (grounded && state[SDL_SCANCODE_W]){
                playerRb->AddForce(Vector2{0.0f, 5.0f});
            }
        }

        currentScene.Update();
        Physics::Update(currentScene);
        GameTime::Update();

        RenderScene(renderer, currentScene);
    }

    DestroyRenderer(window, renderer);

    return 0;
}
