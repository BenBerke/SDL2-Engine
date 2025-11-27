#pragma once
#include "Component.h"
#include "Transform.h"
#include "core/Vector2.h"
#include "config.h"
#include "GameTime.h"
#include <algorithm>

class Rigidbody : public Component {
public:
    Vector2 velocity{0.0f, 0.0f};
    Vector2 gravity;
    bool hasGravity = true;
    float angularVelocity = 0.0f;
    float torque = 0.0f;
    float inertia = 1.0f;
    float invInertia = 1.0f;
    float angularDamping = .99f;
    float linearDamping = .99f;
    bool isKinematic = false;

private:
    Vector2 maxVelocity;

public:
    Rigidbody(Vector2 g = DEFAULT_GRAVITY, Vector2 mv = DEFAULT_MAX_VELOCITY, bool ik = false, bool hg = true)
        : velocity{0.0f, 0.0f},
          gravity(g),
          hasGravity(hg),
          angularVelocity(0.0f),
          torque(0.0f),
          inertia(1.0f),
          invInertia(1.0f),
          angularDamping(.99f),
          linearDamping(.99f),
          isKinematic(ik),
          maxVelocity(mv)
    {}

    ~Rigidbody() = default;

    void Update() override {}

    void AddForce(const Vector2& vel) { velocity += vel; }

    Vector2 GetGravity() const { return gravity; }

    Vector2 GetMaxVelocity() const { return maxVelocity; }

    bool HasGravity() const { return hasGravity; }

    void ApplyTorque(float t) { torque += t; }
};
