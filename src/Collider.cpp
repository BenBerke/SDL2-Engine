#include "pch.h"

#include "Components/Collider.h"

#include "Components/Transform.h"
#include "GameObject.h"

Vector2 Collider::Split(Polygon& poly) {
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

void Collider::Recenter() {
    if (type != ColliderType::Polygon || shape.vertices.empty()) return;
    
    Vector2 centroid = Split(shape);
    for (auto& v : shape.vertices) v -= centroid;

    if (owner) {
        if (auto* t = owner->GetComponent<Transform>()) t->position += centroid;
    }
}

void Collider::OnAttach(GameObject* newOwner) {
    Component::OnAttach(newOwner);
    if (type == ColliderType::Polygon) Recenter();
}
