#pragma once
#include <algorithm>
#include <limits>

union Vec3{
    struct {
    float x;
    float y;
    float z;
    };
    struct {
    float r;
    float g;
    float b;
    };
    Vec3(): x(0), y(0),z(0){}
    Vec3(float xyz) : x(xyz), y(xyz), z(xyz){}
    Vec3(const Vec3 & other) : r(other.r), g(other.g), b(other.b) {
    }
    Vec3( float r, float g, float b) : r(r), g(g), b(b)  {}

    Vec3& operator = (Vec3&& other) {
        if (this != &other ) {
            r = other.r;
            g = other.g;
            b = other.b;
        }
        return *this;
    }

    Vec3& operator=(const Vec3& other) {
        if (this == &other) {
            return *this;
        }

        r = other.r;
        g = other.g;
        b = other.b;

        return *this;
    }

    inline float max() const {
        return  std::max({r, g, b});
    }
    inline float min() const {
        return  std::min({r, g, b});
    }

    inline Vec3& operator /= (float v) {
        if (v > 0) {
            r /= v;
            g /= v;
            b /= v;
        }
        return *this;
    }

    inline Vec3 operator / (float v) const {
        Vec3 res = *this;
        res /= v;
        return res;
    }


    inline Vec3& operator *= (float v) {
        r *= v;
        g *= v;
        b *= v;
        return *this;
    }

    inline Vec3 operator * (float v) const {
        Vec3 res = *this;
        res *= v;
        return res;
    }

    inline Vec3& operator *= (Vec3 v) {
        r *= v.r;
        g *= v.g;
        b *= v.b;
        return *this;
    }

    inline Vec3 operator * (Vec3 v) const {
        Vec3 res = *this;
        res *= v;
        return res;
    }

    inline Vec3& operator += (Vec3 v) {
        r += v.r;
        g += v.g;
        b += v.b;
        return *this;
    }

    inline Vec3 operator + (Vec3 v) const {
        Vec3 res = *this;
        res += v;
        return res;
    }

    inline Vec3& operator -= (Vec3 v) {
        r -= v.r;
        g -= v.g;
        b -= v.b;
        return *this;
    }

    inline Vec3 operator - (Vec3 v) const {
        Vec3 res = *this;
        res -= v;
        return res;
    }

    inline Vec3& operator += (float v) {
        r += v;
        g += v;
        b += v;
        return *this;
    }

    inline Vec3 operator + (float v) const {
        Vec3 res = *this;
        res += v;
        return res;
    }

    inline Vec3& operator -= (float v) {
        return operator+=(-v);
    }

    inline Vec3 operator - (float v) const {
        return operator+(-v);
    }

    inline Vec3 norm() {

        return *this / max();

    }
};

inline Vec3 Mix(Vec3 a, Vec3 b, float v)
{
    return Vec3( map(v,0,1,a.r,b.r),   map(v,0,1,a.g,b.g),  map(v,0,1,a.b,b.b) );

}

inline Vec3 Mix(Vec3 a, Vec3 b, Vec3 c, float ab, float bc)
{
    return  Mix(  Mix(a,b,ab), c, bc);

}

inline Vec3 operator * (float v, const Vec3& rhs) {
        return rhs * v;
}

inline Vec3 operator + (float v, const Vec3& rhs) {
        return rhs + v;
}
