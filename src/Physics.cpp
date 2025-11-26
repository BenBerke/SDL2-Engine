#include "Physics.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "Components/Collider.h"
#include "Components/Rigidbody.h"
#include "GameTime.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Scene.h"

namespace{
std::vector<Vector2> GetAxes(const Polygon& poly){
    std::vector<Vector2> axes;
    if (poly.vertices.size() < 2) return axes;

    for(size_t i = 0; i < poly.vertices.size(); i++){
        Vector2 edge = poly.vertices[i] - poly.vertices[(i + 1) % poly.vertices.size()];
        Vector2 normal = {-edge.y, edge.x};
        normal.Normalize();
        axes.push_back(normal);
    }

    return axes;
}

void ProjectOntoAxis(const Polygon& poly, const Vector2& axis, float& min, float& max){
    if (poly.vertices.empty()){
        min = max = 0.0f;
        return;
    }

    float dot = poly.vertices[0].Dot(axis);
    min = max = dot;
    for(size_t i = 1; i < poly.vertices.size(); i++){
        dot = poly.vertices[i].Dot(axis);
        if(dot < min) min = dot;
        if(dot > max) max = dot;
    }
}
} //namespace 

namespace Physics{
static std::vector<Collider*> colliders;
static std::vector<Rigidbody*> rigidbodies;

static Polygon OffsetShape(const Polygon& shape, const Vector2& position){
    Polygon worldPoly;
    worldPoly.vertices.reserve(shape.vertices.size());
    for(const auto& v : shape.vertices){
        worldPoly.vertices.push_back(v + position);
    }
    return worldPoly;
}

static Polygon ToWorldSpace(const Collider& collider){
    Polygon worldPoly;
    const Transform* transform = collider.owner ? collider.owner->GetComponent<Transform>() : nullptr;
    const Vector2 pos = transform ? transform->position : Vector2{0.0f, 0.0f};
    return OffsetShape(collider.shape, pos);
}

bool Overlaps(float minA, float maxA, float minB, float maxB){
    return !(maxA < minB || maxB < minA);
}

bool CheckCollision(const Polygon& a, const Polygon& b){
    if (a.vertices.empty() || b.vertices.empty()) return false;

    std::vector<Vector2> axesA = GetAxes(a);
    std::vector<Vector2> axesB = GetAxes(b);

    for(auto& axis : axesA){
        float minA, minB, maxA, maxB;
        ProjectOntoAxis(a, axis, minA, maxA);
        ProjectOntoAxis(b, axis, minB, maxB);
        if(!Overlaps(minA, maxA, minB, maxB)) return false;
    }
    for(auto& axis : axesB){
        float minA, minB, maxA, maxB;
        ProjectOntoAxis(a, axis, minA, maxA);
        ProjectOntoAxis(b, axis, minB, maxB);
        if(!Overlaps(minA, maxA, minB, maxB)) return false;
    }

    return true;
}

static bool CollidesAt(const Collider& collider, const Vector2& position){
    if (collider.shape.vertices.empty()) return false;
    if (colliders.empty()) return false;

    const Polygon subject = OffsetShape(collider.shape, position);
    for (Collider* other : colliders){
        if(!other || other == &collider) continue;
        if(!other->owner || !other->owner->isActive) continue;

        const Transform* otherTransform = other->owner->GetComponent<Transform>();
        if(!otherTransform) continue;

        Polygon target = OffsetShape(other->shape, otherTransform->position);
        if (CheckCollision(subject, target)) return true;
    }

    return false;
}

bool Collides(const Collider& collider){
    const Transform* transform = collider.owner ? collider.owner->GetComponent<Transform>() : nullptr;
    if(!transform) return false;
    return CollidesAt(collider, transform->position);
}

void Update(const Scene& scene){
    colliders.clear();
    rigidbodies.clear();

    const auto& objects = scene.GetObjects();
    for (const auto& obj : objects){
        if(!obj || !obj->isActive) continue;
        if(auto collider = obj->GetComponent<Collider>()){
            colliders.push_back(collider);
        }
        if(auto rb = obj->GetComponent<Rigidbody>()){
            rigidbodies.push_back(rb);
        }
    }

    for(size_t i = 0; i < colliders.size(); ++i){
        for(size_t j = i + 1; j < colliders.size(); ++j){
            const Polygon worldA = ToWorldSpace(*colliders[i]);
            const Polygon worldB = ToWorldSpace(*colliders[j]);
            if (CheckCollision(worldA, worldB)){
               // std::cout << "collision detected" << std::endl;
            }
        }
    }

    for (auto* rb : rigidbodies){
        if (!rb || !rb->owner) continue;

        Transform* transform = rb->owner->GetComponent<Transform>();
        Collider* collider = rb->owner->GetComponent<Collider>();
        if(!transform) continue;

        Vector2 velocity = rb->velocity;
        if (rb->HasGravity()){
            velocity.x += rb->GetGravity().x;
            velocity.y += rb->GetGravity().y;
        }

        Vector2 maxVel = rb->GetMaxVelocity();
        velocity.x = std::min(velocity.x, maxVel.x);
        velocity.y = std::min(velocity.y, maxVel.y);

        const Vector2 startPos = transform->position;
        const Vector2 delta = velocity * GameTime::deltaTime;
        Vector2 newPos = startPos;

        auto resolveAxis = [&](float deltaAxis, bool isX){
            if(deltaAxis == 0.0f){
                return;
            }
            Vector2 offset = isX ? Vector2{deltaAxis, 0.0f} : Vector2{0.0f, deltaAxis};
            Vector2 target = newPos + offset;

            if (collider && CollidesAt(*collider, target)){
                Vector2 bestPos = newPos;
                float low = 0.0f, high = 1.0f;
                for (int i = 0; i < 6; ++i){
                    const float mid = (low + high) * 0.5f;
                    Vector2 step = offset * mid;
                    Vector2 candidate = newPos + step;
                    if (CollidesAt(*collider, candidate)){
                        high = mid;
                    }else{
                        low = mid;
                        bestPos = candidate;
                    }
                }
                newPos = bestPos;
                if (isX) velocity.x = 0.0f;
                else velocity.y = 0.0f;
            }else{
                newPos = target;
            }
        };

        resolveAxis(delta.x, true);
        resolveAxis(delta.y, false);

        transform->position = newPos;
        rb->velocity = velocity;
    }
}
} // namespace Physics
