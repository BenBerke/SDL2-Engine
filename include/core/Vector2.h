#pragma once
#include <cmath>

struct Vector2{
    float x, y;
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    template<typename T>
    Vector2 operator*(T value) const { return Vector2(x * value, y * value); }
    Vector2 operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    Vector2 operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Vector2 operator*=(const Vector2& other) {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    Vector2 operator/=(const Vector2& other) {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    bool operator==(const Vector2& other) const {
        if(x == other.x && y == other.y) return true;
        return false;
    }
    bool operator!=(const Vector2& other) const {
        if(x == other.x && y == other.y) return false;
        return true;
    }

    float Length() const{ return std::sqrt(x*x + y*y); }
    Vector2 Normalized() const { 
        float len = Length();
        return len == 0 ? Vector2() : Vector2(x / len, y / len); 
    }
    void Normalize(){
        float len = Length();
        if(len == 0) return;
        x/=len;
        y/=len;
    }
    float Dot(const Vector2& other) const{
        return x * other.x + y * other.y;
    }
};