#pragma once
#include <utility>
#include <vector>
#include <limits>
#include <cmath>

#include "Component.h"
#include "core/Vector2.h"

struct Polygon {
    std::vector<Vector2> vertices;
};

struct Circle {
    float radius = 0.0f;
};

enum class ColliderType {
    Polygon,
    Circle
};

class Collider : public Component {
public:
    ColliderType type = ColliderType::Polygon; 

    Polygon shape;   
    Circle circle;   

    bool isTrigger = false; 

    Collider() = default;

    explicit Collider(const Polygon& polygon) 
        : type(ColliderType::Polygon), shape(polygon) {}

    explicit Collider(float radius)
        : type(ColliderType::Circle) {
        circle.radius = radius;
    }

    ~Collider() override = default;

    void Update() override {}
};

