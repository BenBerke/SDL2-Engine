#include <iostream>
#include <math.h>

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

void RenderScene(SDL_Renderer* renderer, Scene& scene, SDL_Color clearColor, SDL_Point pivot)
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

        SDL_Point renderPivot = pivot;
        if (renderPivot.x < 0 || renderPivot.y < 0)
        {
            renderPivot.x = rect.w / 2;
            renderPivot.y = rect.h / 2;
        }

        float rotationRadians = 0.0f;
        float rotationDegrees = 0.0f;
        Transform* transform = objectPtr->GetComponent<Transform>();
        if (transform)
        {
            rotationRadians = transform->rotation;
            rotationDegrees = rotationRadians * (180.0f / static_cast<float>(M_PI));
        }

        if (sprite->HasTexture())
        {
            SDL_Texture* texture = sprite->GetTexture();
            if (!texture) continue;

            SDL_RenderCopyEx(renderer, texture, nullptr, &rect, rotationDegrees, &renderPivot, SDL_FLIP_NONE);
        }
        else
        {
            SDL_Color drawColor{};
            SDL_GetRenderDrawColor(renderer, &drawColor.r, &drawColor.g, &drawColor.b, &drawColor.a);

            const SDL_FPoint pivotWorld = {
                static_cast<float>(rect.x + renderPivot.x),
                static_cast<float>(rect.y + renderPivot.y)
            };

            SDL_FPoint corners[4] = {
                {static_cast<float>(rect.x), static_cast<float>(rect.y)},
                {static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y)},
                {static_cast<float>(rect.x + rect.w), static_cast<float>(rect.y + rect.h)},
                {static_cast<float>(rect.x), static_cast<float>(rect.y + rect.h)}
            };

            const float cosAngle = cosf(rotationRadians);
            const float sinAngle = sinf(rotationRadians);

            SDL_Vertex vertices[4];
            for (int i = 0; i < 4; ++i)
            {
                const float dx = corners[i].x - pivotWorld.x;
                const float dy = corners[i].y - pivotWorld.y;
                vertices[i].position.x = pivotWorld.x + dx * cosAngle - dy * sinAngle;
                vertices[i].position.y = pivotWorld.y + dx * sinAngle + dy * cosAngle;
                vertices[i].color = drawColor;
                vertices[i].tex_coord = {0.0f, 0.0f};
            }

            const int indices[6] = {0, 1, 2, 2, 3, 0};
            SDL_RenderGeometry(renderer, nullptr, vertices, 4, indices, 6);
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
