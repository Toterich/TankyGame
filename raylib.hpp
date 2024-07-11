#pragma once

namespace rl {
	#include <raylib.h>

    Vector2 operator+(const Vector2& a, const Vector2& b) {
        return Vector2{.x=a.x + b.x, .y=a.y + b.y};
    }

    Vector2 operator-(const Vector2& a, const Vector2& b) {
        return Vector2{.x=a.x - b.x, .y=a.y - b.y};
    }

    Vector2& operator+=(Vector2& a, const Vector2& b) {
        a.x += b.x;
        a.y += b.y;
        return a;
    }

    Vector2& operator-=(Vector2& a, const Vector2& b) {
        a.x -= b.x;
        a.y -= b.y;
        return a;
    }
}
