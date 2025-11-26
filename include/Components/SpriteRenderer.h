#pragma once
#include "Component.h"

#include "Transform.h"
#include "config.h"
#include "core/Vector2.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

class SpriteRenderer : public Component{
private:
    Transform* transform;
    Vector2 size;
    SDL_Texture* texture;
public:
    SpriteRenderer(Vector2 s = Vector2{1, 1}) : transform(nullptr), size(s), texture(nullptr) {}

    bool BuildRect(SDL_Rect& outRect){
        if (!transform && owner) {
            transform = owner->GetComponent<Transform>();
        }
        if (!transform) return false;

        const Vector2 screenPos = transform->GetScreenPosition();
        const float widthPixels = size.x * PIXELS_PER_UNIT * transform->scale.x;
        const float heightPixels = size.y * PIXELS_PER_UNIT * transform->scale.y;

        outRect.x = static_cast<int>(screenPos.x - widthPixels * 0.5f);
        outRect.y = static_cast<int>(screenPos.y - heightPixels * 0.5f);
        outRect.w = static_cast<int>(widthPixels);
        outRect.h = static_cast<int>(heightPixels);
        return true;
    }

    SDL_Texture* GetTexture() const {
        return texture;
    }

    void SetTexture(SDL_Texture* tex) {
        texture = tex;
    }

    bool HasTexture() const {
        return texture != nullptr;
    }

    void Update() override{
        if (!transform && owner) {
            transform = owner->GetComponent<Transform>();
        }
    }
};
