#include <iostream>

#include "Renderer.h"
#include "Scene.h"
#include "Components/SpriteRenderer.h"

bool InitRenderer(SDL_Window*& window, SDL_Renderer*& renderer)
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cout << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    window = SDL_CreateWindow(
        "Hello, World!",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == nullptr)
    {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr)
    {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        window = nullptr;
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    return true;
}

void BeginFrame(SDL_Renderer* renderer, SDL_Color clearColor)
{
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    SDL_RenderClear(renderer);
}

void RenderScene(SDL_Renderer* renderer, Scene& scene, SDL_Color clearColor)
{
    if (!renderer) return;

    BeginFrame(renderer, clearColor);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    for (const auto& objectPtr : scene.GetObjects())
    {
        if (!objectPtr || !objectPtr->isActive) continue;

        SpriteRenderer* sprite = objectPtr->GetComponent<SpriteRenderer>();
        if (!sprite) continue;

        SDL_Rect rect{};
        if (!sprite->BuildRect(rect)) continue;

        if (sprite->HasTexture())
        {
            SDL_Texture* texture = sprite->GetTexture();
            if (!texture) continue;
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        }
        else
        {
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    EndFrame(renderer);
}

void EndFrame(SDL_Renderer* renderer)
{
    if (!renderer) return;

    SDL_RenderPresent(renderer);
}

void DestroyRenderer(SDL_Window* window, SDL_Renderer* renderer)
{
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();
}
