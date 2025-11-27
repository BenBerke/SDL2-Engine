#pragma once
#include <utility>
#include <vector>
#include <limits>
#include <cmath>

#include "Component.h"
#include "Transform.h"
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

    Vector2 Split(Polygon& poly) {
        std::vector<float> areas;
        std::vector<Vector2> centroids;
        areas.reserve(poly.vertices.size());
        centroids.reserve(poly.vertices.size());

        float totalArea = 0.0f;
        float sumX = 0.0f;
        float sumY = 0.0f;

        for (size_t i = 0; i < poly.vertices.size(); ++i) {
            size_t j = (i + 1) % poly.vertices.size();

            float currentArea = 0.5f * (poly.vertices[i].x * poly.vertices[j].y -
                                        poly.vertices[j].x * poly.vertices[i].y);
            Vector2 currentCentroid = {
                (poly.vertices[i].x + poly.vertices[j].x) / 3.0f,
                (poly.vertices[i].y + poly.vertices[j].y) / 3.0f
            };

            totalArea += currentArea;
            sumX += currentCentroid.x * currentArea;
            sumY += currentCentroid.y * currentArea;

            areas.push_back(currentArea);
            centroids.push_back(currentCentroid);
        }
        Vector2 centroid = { sumX / totalArea, sumY / totalArea };
        return centroid;
    }

    void Recenter() {
        if (type != ColliderType::Polygon || shape.vertices.empty()) return;
        
        Vector2 centroid = Split(shape);
        for (auto& v : shape.vertices) v -= centroid;

        if (owner) {
            if (auto* t = owner->GetComponent<Transform>()) t->position += centroid;
        }
    }

    void OnAttach(GameObject* newOwner) override {
        Component::OnAttach(newOwner);
        if (type == ColliderType::Polygon) Recenter();
    }
};
