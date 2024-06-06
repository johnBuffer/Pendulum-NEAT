#pragma once
#include <algorithm>
#include <cmath>

#undef min
#undef max

struct Math
{
    template<typename TFloat>
    struct Constant
    {
        static constexpr TFloat Pi = static_cast<TFloat>(3.141592653589793238462643383279502884197);
        static constexpr TFloat TwoPi = 2.0 * Pi;
    };

    using ConstantF32 = Constant<float>;
    using ConstantF64 = Constant<double>;

    static float pow(float v, const uint32_t p)
    {
        float res = 1;
        for (uint32_t i{p}; i--;) {
            res *= v;
        }
        return res;
    }

    template<typename T>
    static T sign(T v)
    {
        return v < 0.0f ? -1.0f : 1.0f;
    }
    
    static float sigm(float x)
    {
        return 1.0f / (1.0f + exp(-x));
    }
    
    static float sigm_0(float x)
    {
        return sigm(x) - 0.5f;
    }

    template<typename TFloat>
    static TFloat radToDeg(TFloat r)
    {
        constexpr TFloat radian_to_deg = TFloat{180.0} / Constant<TFloat>::Pi;
        return r * radian_to_deg;
    }

    static float clamp(float v, float min, float max)
    {
        return std::min(std::max(min, v), max);
    }

    static float gaussian(float x, float a, float b, float c)
    {
        const float n = x-b;
        return a * std::exp(-(n * n)/(2.0f * c * c));
    }

    static float clampAmplitude(float x, float maximum)
    {
        return sign(x) * std::min(std::abs(x), maximum);
    }
};


struct MathVec2
{
    template<template<typename> class Vec2Type, typename TFloat>
    static TFloat length2(const Vec2Type<TFloat> v)
    {
        return v.x * v.x + v.y * v.y;
    }

    template<template<typename> class Vec2Type, typename TFloat>
    static TFloat length(const Vec2Type<TFloat> v)
    {
        return sqrt(length2(v));
    }

    template<typename TVec2>
    static float angle(TVec2 v_1, TVec2 v_2 = {1.0f, 0.0f})
    {
        const float dot = v_1.x * v_2.x + v_1.y * v_2.y;
        const float det = v_1.x * v_2.y - v_1.y * v_2.x;
        return atan2(det, dot);
    }
    
    template<template<typename> class Vec2Type, typename TFloat>
    static TFloat dot(Vec2Type<TFloat> v1, Vec2Type<TFloat> v2)
    {
        return v1.x * v2.x + v1.y * v2.y;
    }

    template<template<typename> class Vec2Type, typename TFloat>
    static TFloat cross(const Vec2Type<TFloat> v1, const Vec2Type<TFloat> v2)
    {
        return v1.x * v2.y - v1.y * v2.x;
    }
    
    template<typename Vec2Type>
    static Vec2Type normal(const Vec2Type& v)
    {
        return {-v.y, v.x};
    }

    template<typename Vec2Type>
    static Vec2Type rotate(const Vec2Type& v, float angle)
    {
        const float ca = cos(angle);
        const float sa = sin(angle);
        return {ca * v.x - sa * v.y, sa * v.x + ca * v.y};
    }

    template<typename Vec2Type>
    static Vec2Type rotateDir(const Vec2Type& v, const Vec2Type& dir)
    {
        return { dir.x * v.x - dir.y * v.y, dir.y * v.x + dir.x * v.y };
    }

    template<typename Vec2Type>
    static Vec2Type normalize(const Vec2Type& v)
    {
        return v / length(v);
    }

    template<template<typename> class Vec2Type, typename T>
    static Vec2Type<T> reflect(const Vec2Type<T>& v, const Vec2Type<T>& n)
    {
        return v - n * (MathVec2::dot(v, n) * 2.0f);
    }
};

