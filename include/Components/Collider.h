#pragma once
#include <vector>

#include "Component.h"
#include "core/Vector.h"

class Transform;


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
    bool drawGizmo = false;
    Vector2 scale = {1.0f, 1.0f};

    Collider() = default;

    explicit Collider(const Polygon& polygon) 
        : type(ColliderType::Polygon), shape(polygon) {}

    explicit Collider(float radius)
        : type(ColliderType::Circle) {
        circle.radius = radius;
    }

    ~Collider() override = default;

    void Update() override {}

    Vector2 Split(Polygon& poly);

    void Recenter();

    void OnAttach(GameObject* newOwner) override;
};
