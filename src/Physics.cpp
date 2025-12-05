#include "Physics.h"

#include "pch.h"

#include "core/Vector.h"
#include "core/Matrix3x3.h"

#include "Components/Collider.h"
#include "Components/Rigidbody.h"

#include "config.h"
#include "Components/CustomBehaviour.h"
#include "GameTime.h"
#include "GameObject.h"
#include "Components/Transform.h"
#include "Scene.h"

struct SpatialGrid {
    float cellSize = SPATIAL_GRID_SIZE;
    std::unordered_map<int64_t, std::vector<Collider*>> cells;

    void Clear() {cells.clear(); }

    int64_t Hash(int x, int y) const { return ((int64_t)x << 32) ^ (int64_t)y; }

    void Insert(Collider* collider, const Vector2& pos){
        int gx = static_cast<int>(std::floor(pos.x / cellSize)); 
        int gy = static_cast<int>(std::floor(pos.y / cellSize)); 
        cells[Hash(gx, gy)].push_back(collider);
    }

    std::vector<std::vector<Collider*>> GetNeighborCells(int gx, int gy) const{
        std::vector<std::vector<Collider*>> neighbors;
        neighbors.reserve(5);

        for(int dy = -1; dy <= 1; ++dy){
            for(int dx = -1; dx <= 1; ++dx){
                int64_t h = Hash(gx + dx, gy + dy);
                auto it = cells.find(h);
                if(it != cells.end()) neighbors.push_back(it->second);
            }
        }

        return neighbors;
    }
};

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

std::pair<Collider*, Collider*> MakeOrderedPair(Collider* a, Collider* b){
    return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
}

void NotifyCollision(Collider* self, Collider* other, void (CustomBehaviour::*fn)(Collider*)){
    if (!self) return;
    GameObject* owner = self->owner;
    if (!owner || !owner->isActive) return;

    auto behaviours = owner->GetComponents<CustomBehaviour>();
    for (auto* behaviour : behaviours){
        if (!behaviour) continue;
        (behaviour->*fn)(other);
    }
}
} //namespace 

Vector2 TransformPoint(const Vector2& point, const Matrix3x3 mat){
    float x = point.x * mat.m[0][0] + point.y * mat.m[0][1] + mat.m[0][2];
    float y = point.x * mat.m[1][0] + point.y * mat.m[1][1] + mat.m[1][2];

    return {x, y};
}

namespace Physics{
static std::vector<Collider*> colliders;
static std::vector<Rigidbody*> rigidbodies;
static std::vector<std::pair<Collider*, Collider*>> previousCollisions;

static Polygon ToWorldSpaceFull(const Collider& collider, const Vector2& overridePos = Vector2{0.0f, 0.0f})
{
    Polygon worldPoly;
    const Transform* transform = collider.owner ? collider.owner->GetComponent<Transform>() : nullptr;
    if (!transform) return worldPoly;

    Matrix3x3 mat = transform->GetMatrix();

    if (overridePos != Vector2{0, 0}) {
        mat.m[0][2] = overridePos.x;
        mat.m[1][2] = overridePos.y;
    }

    worldPoly.vertices.reserve(collider.shape.vertices.size());
    for (const auto& v : collider.shape.vertices)
        worldPoly.vertices.push_back(TransformPoint(v, mat));

    return worldPoly;
}


static Polygon ToWorldSpace(const Collider& collider){
    Polygon worldPoly;
    const Transform* transform = collider.owner ? collider.owner->GetComponent<Transform>() : nullptr;
    if(!transform) return worldPoly;

    Matrix3x3 mat = transform->GetMatrix();
    worldPoly.vertices.reserve(collider.shape.vertices.size());

    for(const auto& v : collider.shape.vertices){
        worldPoly.vertices.push_back(TransformPoint(v, mat));
    }

    return worldPoly;
}

bool Overlaps(float minA, float maxA, float minB, float maxB){
    return !(maxA < minB || maxB < minA);
}

bool CheckCollision(const Polygon& a, const Polygon& b){
    if (a.vertices.empty() || b.vertices.empty()) return false;

    auto axes = GetAxes(a);
    const auto axesB = GetAxes(b);
    axes.insert(axes.end(), axesB.begin(), axesB.end());

    for (const auto& axis : axes){
        if (axis.Length() == 0.0f) continue;
        float minA, maxA, minB, maxB;
        ProjectOntoAxis(a, axis, minA, maxA);
        ProjectOntoAxis(b, axis, minB, maxB);
        if (!Overlaps(minA, maxA, minB, maxB)){
            return false;
        }
    }

    return true;
}

bool CircleCircleCollision(const Vector2& aPos, float aR, const Vector2& bPos, float bR){
    Vector2 diff = bPos - aPos;
    float distSq = diff.x * diff.x + diff.y * diff.y;
    float rSum = aR + bR;
    return distSq <= (rSum * rSum);
}

Vector2 ClosestPointOnPolygon(Vector2 point, const Polygon& poly){
    if (poly.vertices.empty()) return point;

    float minDist = std::numeric_limits<float>::max();
    Vector2 closest = poly.vertices[0];
    for(size_t i = 0; i <poly.vertices.size(); i++){
        Vector2 a = poly.vertices[i];
        Vector2 b = poly.vertices[(i + 1) % poly.vertices.size()];
        Vector2 ab = b - a;
        float denom = ab.Dot(ab);
        if (denom == 0.0f) continue;
        float t = ((point - a ).Dot(ab)) / denom;
        t = std::clamp(t, 0.0f, 1.0f);
        Vector2 candidate = a + ab * t;
        float dist = (candidate - point).Length();
        if(dist < minDist) {minDist = dist; closest = candidate;}
    }

    return closest;
}

bool CirclePolygonCollision(const Vector2& circleCenter, float radius, const Polygon& poly){
    if(poly.vertices.empty()) return false;
    if(radius <= 0.0f) return false;

    auto axes = GetAxes(poly);
    Vector2 closest = ClosestPointOnPolygon(circleCenter, poly);
    Vector2 axis = closest - circleCenter;
    const float axisLen = axis.Length();
    if(axisLen > 0) {
        axis.Normalize();
        axes.push_back(axis);
    }

    for(auto& ax : axes){
        if (ax.Length() == 0.0f) continue;
        float minPoly, maxPoly;
        ProjectOntoAxis(poly, ax, minPoly, maxPoly);

        float c = circleCenter.Dot(ax);
        float minCircle = c - radius;
        float maxCircle = c + radius;

        if(!Overlaps(minPoly, maxPoly, minCircle, maxCircle)) return false;
    }

    return true;
}

bool CheckCollision(const Collider& a, const Collider& b){
    const Transform* tA = a.owner ? a.owner->GetComponent<Transform>() : nullptr;
    const Transform* tB = b.owner ? b.owner->GetComponent<Transform>() : nullptr;
    Vector2 posA = tA ? tA->position : Vector2{};
    Vector2 posB = tB ? tB->position : Vector2{};

    if(a.type == ColliderType::Circle && b.type == ColliderType::Circle)
    return CircleCircleCollision(posA, a.circle.radius, posB, b.circle.radius);

    if(a.type == ColliderType::Circle && b.type == ColliderType::Polygon)
    return CirclePolygonCollision(posA, a.circle.radius, ToWorldSpace(b));

    if(a.type == ColliderType::Polygon && b.type == ColliderType::Circle)
    return CirclePolygonCollision(posB, b.circle.radius, ToWorldSpace(a));

    return CheckCollision(ToWorldSpace(a), ToWorldSpace(b));
}

static bool CollidesAt(const Collider& collider, const Vector2& position){
    if (colliders.empty()) return false;

    const bool isCircle  = collider.type == ColliderType::Circle;
    const bool isPolygon = collider.type == ColliderType::Polygon;
    if (isPolygon && collider.shape.vertices.empty()) return false;
    if (isCircle && collider.circle.radius <= 0.0f)   return false;

    Polygon subjectPoly;
    if (isPolygon){
        // Moving collider at *test* position
        subjectPoly = ToWorldSpaceFull(collider, position);
    }

    for (Collider* other : colliders){
        if (!other || other == &collider) continue;
        if (!other->owner || !other->owner->isActive) continue;

        const Transform* otherTransform = other->owner->GetComponent<Transform>();
        if (!otherTransform) continue;

        const Vector2 otherPos = otherTransform->position;

        if (isCircle){
            if (other->type == ColliderType::Circle){
                if (other->circle.radius <= 0.0f) continue;
                if (CircleCircleCollision(position, collider.circle.radius,
                                          otherPos, other->circle.radius))
                    return true;
            } else { // circle vs polygon
                Polygon target = ToWorldSpaceFull(*other);
                if (CirclePolygonCollision(position, collider.circle.radius, target))
                    return true;
            }
        } else { // subject is polygon
            if (other->type == ColliderType::Circle){
                if (other->circle.radius <= 0.0f) continue;
                if (CirclePolygonCollision(otherPos, other->circle.radius, subjectPoly))
                    return true;
            } else { // polygon vs polygon
                Polygon target = ToWorldSpaceFull(*other);
                if (CheckCollision(subjectPoly, target))
                    return true;
            }
        }
    }

    return false;
}

bool Collides(const Collider& collider){
    const Transform* transform = collider.owner ? collider.owner->GetComponent<Transform>() : nullptr;
    if(!transform) return false;
    return CollidesAt(collider, transform->position);
}

SpatialGrid grid;

void Update(const Scene& scene){
    colliders.clear();
    rigidbodies.clear();
    grid.Clear();

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

    for(Collider* c : colliders){
        Transform* t = c->owner->GetComponent<Transform>();
        if (!t) continue;
        grid.Insert(c, t->position);
    }


    previousCollisions.erase(
        std::remove_if(previousCollisions.begin(), previousCollisions.end(),
                       [&](const auto& pair){
                           return !pair.first || !pair.second ||
                                  std::find(colliders.begin(), colliders.end(), pair.first) == colliders.end() ||
                                  std::find(colliders.begin(), colliders.end(), pair.second) == colliders.end();
                       }),
        previousCollisions.end());

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
        velocity.x = std::clamp(velocity.x, -maxVel.x, maxVel.x);
        velocity.y = std::clamp(velocity.y, -maxVel.y, maxVel.y);

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

        if(rb->inertia > 0.0f){
            rb->angularVelocity += rb->torque * rb->invInertia * GameTime::deltaTime;
            rb->torque = 0.0f;
        }
        transform->rotation += rb->angularVelocity * GameTime::deltaTime; 
        rb->angularVelocity *= std::pow(rb->angularDamping, GameTime::deltaTime * 60.0f);
        rb->velocity *= rb->linearDamping;

        transform->position = newPos;
        rb->velocity = velocity;
    }

    std::vector<std::pair<Collider*, Collider*>> currentCollisions;
    for(auto& [cellkey, list] : grid.cells){
        int gx = static_cast<int>(cellkey >> 32);
        int gy = static_cast<int>(cellkey & 0xFFFFFFFF);

        auto neighborsList = grid.GetNeighborCells(gx, gy);

        for(Collider* a : list){
            for(const auto& nlist : neighborsList){
                for(Collider* b : nlist){
                    if(a == b) continue;
                    if(a < b){
                        if(CheckCollision(*a, *b)) currentCollisions.push_back(MakeOrderedPair(a, b));
                    }
                }
            }
        }
    }
    

    for (const auto& pair : currentCollisions){
        const bool wasColliding = std::find(previousCollisions.begin(), previousCollisions.end(), pair) != previousCollisions.end();
        if (wasColliding){
            NotifyCollision(pair.first, pair.second, &CustomBehaviour::OnCollisionStay);
            NotifyCollision(pair.second, pair.first, &CustomBehaviour::OnCollisionStay);
        }else{
            NotifyCollision(pair.first, pair.second, &CustomBehaviour::OnCollisionEnter);
            NotifyCollision(pair.second, pair.first, &CustomBehaviour::OnCollisionEnter);
        }
    }

    for (const auto& pair : previousCollisions){
        const bool stillColliding = std::find(currentCollisions.begin(), currentCollisions.end(), pair) != currentCollisions.end();
        if (!stillColliding){
            NotifyCollision(pair.first, pair.second, &CustomBehaviour::OnCollisionExit);
            NotifyCollision(pair.second, pair.first, &CustomBehaviour::OnCollisionExit);
        }
    }

    previousCollisions = std::move(currentCollisions);
}
} // namespace Physics
