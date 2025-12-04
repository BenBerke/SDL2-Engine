#include <iostream>
#include <glad/glad.h>

#include "Renderer.h"

#include "GameObject.h"
#include "config.h"
#include "Scene.h"
#include "GameTime.h"
#include "Physics.h"
#include "InputManager.h"

#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/SpriteRenderer.h"
#include "Components/Rigidbody.h"
#include "Components/BoxCollider.h"
#include "Components/CircleCollider.h"
#include "Components/CustomBehaviour.h"

#include "CustomBehaviours/TestBehaviour.h"

#include <SDL2/SDL.h>

int main()
{
    SDL_Window* window = nullptr;
    SDL_GLContext glContext;

    if (!InitWindow(window, glContext))
        return -1;

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            running = false;
        }


        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Dark gray background
        glClear(GL_COLOR_BUFFER_BIT);

        DrawTriangle();

        SDL_GL_SwapWindow(window);
    }

    DestroyWindow(window, glContext);
    return 0;
}