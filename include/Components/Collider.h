#pragma once
#include <utility>
#include <vector>

#include "Component.h"
#include "core/Vector2.h"

struct Polygon{
    std::vector<Vector2> vertices;
};

class Collider : public Component{
public:
    Polygon shape;

    Collider() = default;
    explicit Collider(Polygon polygon) : shape(std::move(polygon)) {}
    ~Collider() override = default;

    void Update() override {}
};
