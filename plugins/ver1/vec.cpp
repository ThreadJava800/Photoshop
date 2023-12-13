#include "plugin.h"

#include <math.h>

static const double EPSILON = 1e-12;

void plugin::Vec2::operator+=(const plugin::Vec2&  b) {
    this->x += b.x;
    this->y += b.y;
}

plugin::Vec2 plugin::Vec2::operator+ (const plugin::Vec2&  b) {
    plugin::Vec2 res = *this;
    res += b;

    return res;
}

void plugin::Vec2::operator-=(const plugin::Vec2&  b) {
    this->x -= b.x;
    this->y -= b.y;
}

plugin::Vec2 plugin::Vec2::operator- (const plugin::Vec2&  b) {
    plugin::Vec2 res = *this;
    res -= b;

    return res;
}

void plugin::Vec2::operator*=(const double b) {
    this->x *= b;
    this->y *= b;
}

plugin::Vec2 plugin::Vec2::operator* (const double b) {
    plugin::Vec2 res = *this;
    res *= b;

    return res;
}

bool plugin::Vec2::operator==(const plugin::Vec2&  b) {
    return fabs(this->x - b.x) < EPSILON && fabs(this->y - b.y) < EPSILON;
}

double plugin::Vec2::operator|(const plugin::Vec2& b) {
    return this->x * b.x + this->y * b.y;
}
