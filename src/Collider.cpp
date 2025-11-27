#include <iostream>

#include "Collider.h"

#include "core/Vector2.h"

void Split(Polygon& poly) {
    std::vector<float> areas;
    std::vector<Vector2> centroids;
    areas.reserve(poly.vertices.size());
    centroids.reserve(poly.vertices.size());

    float totalArea = 0.0f;
    float sumX = 0.0f;
    float sumY = 0.0f;

    for (int i = 0; i < poly.vertices.size(); ++i) {
        int j = (i + 1) % poly.vertices.size();

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
}