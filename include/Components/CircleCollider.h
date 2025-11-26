#pragma once
#include "Collider.h"

class CircleCollider : public Collider{
public:
    explicit CircleCollider(float radius = 1.0f){
        type = ColliderType::Circle;
        circle.radius = radius;
        shape.vertices.clear();
    }
    ~CircleCollider() override = default;
};
