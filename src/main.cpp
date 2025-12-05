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

// #include <box2d/box2d.h>
// #include <iostream>

// int main() {
//    float timeStep = 1.0f / 60.0f;
//    int subStepCount = 4;

//    b2WorldDef worldDef = b2DefaultWorldDef();
//    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
//    b2WorldId worldId = b2CreateWorld(&worldDef);

//    b2BodyDef groundBodyDef = b2DefaultBodyDef();
//    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};
//    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);
//    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
//    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
//    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

//    b2BodyDef bodyDef = b2DefaultBodyDef();
//    bodyDef.type = b2_dynamicBody;
//    bodyDef.position = {0.0f, 4.0f};
//    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);
//    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);
//    b2ShapeDef shapeDef = b2DefaultShapeDef();
//    shapeDef.density = 1.0f;
//    shapeDef.material.friction = 0.3f;
//    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);

//    for (int i = 0; i < 90; ++i)
//    {
//     b2World_Step(worldId, timeStep, subStepCount);
//     b2Vec2 position = b2Body_GetPosition(bodyId);
//     b2Rot rotation = b2Body_GetRotation(bodyId);
//     printf("%4.2f %4.2f %4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));
//    }

//    b2DestroyWorld(worldId);
// }

