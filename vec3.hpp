#pragma once
#include <algorithm>
#include <limits>

namespace MyVec {
template <typename Float>
union Vec3{
    using Type = Float;
    static constexpr auto epsilon = std::numeric_limits<Float>::epsilon();
    struct {
    Float x;
    Float y;
    Float z;
    };
    struct {
    Float r;
    Float g;
    Float b;
    };
    Vec3(): x(0), y(0),z(0){}
    Vec3(const Vec3 & other) : r(other.r), g(other.g), b(other.b) {
    }
    Vec3(Float xyz) : x(xyz), y(xyz), z(xyz){}
    Vec3(Float r, Float g, Float b) : r(r), g(g), b(b)  {}
    template <typename FloatCast>
    Vec3(const Vec3<FloatCast> & other) : r(Float(other.r)), g(Float(other.g)), b(Float(other.b)) {
    }


    template <typename FloatCast>
    Vec3(FloatCast xyz) : x(Float(xyz)), y(Float(xyz)), z(Float(xyz)){}
    template <typename FloatCast>
    Vec3(FloatCast r, FloatCast g, FloatCast b) : r(Float(r)), g(Float(g)), b(Float(b))  {}

    inline Vec3& operator = (Vec3&& other) {
        if (this != &other ) {
            r = other.r;
            g = other.g;
            b = other.b;
        }
        return *this;
    }

    // Оператор копирующего присваивания
    inline Vec3& operator=(const Vec3& other) {
        if (this == &other) {
            return *this;
        }

        // Копируем члены данных
        r = other.r;
        g = other.g;
        b = other.b;

        return *this;
    }

    inline Float max() const {
        return  std::max({r, g, b});
    }
    inline Float min() const {
        return  std::min({r, g, b});
    }


    inline bool operator==(const Vec3& other) const {
        return std::abs(x - other.x) < epsilon && std::abs(y - other.y) < epsilon && fabs(z - other.z) < epsilon;
    }
    inline bool operator!=(const Vec3& other) const {
        return !(other == *this);
    }

    inline bool operator<(const Vec3& other) const {
        return other != *this && x < other.x && y < other.y && z < other.z;
    }
    inline bool operator>(const Vec3& other) const {
        return other < *this;
    }

    inline bool operator<=(const Vec3& other) const {
        return *this==other || *this < other;
    }

    inline bool operator>=(const Vec3& other) const {
        return *this==other || *this > other;
    }

    inline Vec3 operator-() const {
        return {-r,-g,-b};
    }



    inline Vec3& operator /= (Float v) {
        r /= v;
        g /= v;
        b /= v;
        return *this;
    }

    inline Vec3 operator / (Float v) const {
        Vec3 res = *this;
        res /= v;
        return res;
    }


    inline Vec3& operator *= (Float v) {
        r *= v;
        g *= v;
        b *= v;
        return *this;
    }

    inline Vec3 operator * (Float v) const {
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

    inline Vec3& operator += (Float v) {
        r += v;
        g += v;
        b += v;
        return *this;
    }

    inline Vec3 operator + (Float v) const {
        Vec3 res = *this;
        res += v;
        return res;
    }

    inline Vec3& operator -= (Float v) {
        return operator+=(-v);
    }

    inline Vec3 operator - (Float v) const {
        return operator+(-v);
    }

    inline Vec3 norm() {

        return *this / max();

    }

    friend inline Vec3 operator * (Float v, const Vec3& rhs) {
            return rhs * v;
    }


    friend inline Vec3 operator + (Float v, const Vec3& rhs) {
            return rhs + v;
    }

};

template <typename T, typename T2, typename T3>
inline constexpr auto clip(T v, T2 minValue, T3 maxValue) {

  return std::max<T>(minValue, std::min<T>(v, maxValue));
}


template <typename T, typename T2, typename T3>
inline constexpr Vec3<T> clip(Vec3<T> v, T2 minValue, T3 maxValue) {
    v.x = clip(minValue, std::min(v.x, maxValue));
    v.y = clip(minValue, std::min(v.y, maxValue));
    v.z = clip(minValue, std::min(v.z, maxValue));
    return v;
}

template <typename In>
constexpr inline auto lerp(In start, In end, float t)
{
    return clip(t,0,1) * (end - start) + start;
}


template <typename Float>
inline Vec3<Float> Mix(Vec3<Float> a, Vec3<Float> b, Float v)
{
    return lerp(a,b,v);

}

template <typename Float>
inline Vec3<Float> Mix(Vec3<Float> a, Vec3<Float> b, Vec3<Float> c, float ab, float bc)
{
    return  Mix(  Mix(a,b,ab), c, bc);
}    

}


using Vec3 = MyVec::Vec3<float>;

