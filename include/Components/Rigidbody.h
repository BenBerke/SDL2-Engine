#pragma once
#include "Component.h"
#include "Transform.h"
#include "core/Vector2.h"
#include "config.h"
#include "GameTime.h"
#include <algorithm>

class Rigidbody : public Component{
public:
    Vector2 velocity{0.0f, 0.0f};

private:
    Vector2 maxVelocity;
    Vector2 gravity;

    bool isKinematic;
    bool hasGravity;

public:
    Rigidbody(Vector2 g = DEFAULT_GRAVITY, Vector2 mv = DEFAULT_MAX_VELOCITY, bool ik = false, bool hg = true)
     : velocity{0.0f, 0.0f}, maxVelocity(mv), gravity(g), isKinematic(ik), hasGravity(hg) {}
    ~Rigidbody() = default;

    void Update() override{
        // Movement and collision handled centrally by Physics.
    }

    void AddForce(Vector2 vel){
        velocity += vel;
    }

    Vector2 GetGravity() const{
        return gravity;
    }

    Vector2 GetMaxVelocity() const{
        return maxVelocity;
    }

    bool HasGravity() const{
        return hasGravity;
    }

};
