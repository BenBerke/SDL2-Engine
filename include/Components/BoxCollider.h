#pragma once
#include "Collider.h"

class BoxCollider : public Collider{
public:
    explicit BoxCollider(float width = 1.0f, float height = 1.0f){
        const float halfW = width * 0.5f;
        const float halfH = height * 0.5f;
        shape.vertices = {
            {-halfW, -halfH},
            { halfW, -halfH},
            { halfW,  halfH},
            {-halfW,  halfH}
        };
    }
    ~BoxCollider() override = default;
};
