#pragma once

class Scene;
struct Polygon;
struct Vector2;
class Collider;
class Rigidbody;

namespace Physics{
    void Update(const Scene& scene);

    bool Overlaps(float minA, float maxA, float minB, float maxB);
    bool CheckCollision(const Polygon& a, const Polygon& b);
    bool Collides(const Collider& collider);
}
