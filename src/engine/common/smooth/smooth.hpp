#pragma once
#include <cstdint>
#include "engine/common/math.hpp"


// The easing function to use
enum class Interpolation
{
    None,
    Linear,
    EaseInOutExponential,
    EaseInOutCirc,
    EaseInOutQuint,
    EaseOutBack,
    EaseOutElastic,
    Sigmoid
};

struct Smooth
{
    static float flip(float x)
    {
        return 1 - x;
    }

    static float dumbPow(float x, uint32_t p)
    {
        float res = 1.0f;
        for (uint32_t i(p); i--;) {
            res *= x;
        }
        return res;
    }

    static float clampTime(float t)
    {
        return std::min(1.0f, std::max(t, 0.0f));
    }

    template<typename TValueType>
    static TValueType mix(const TValueType& a, const TValueType& b, float ratio)
    {
        return (1.0f - ratio) * a + ratio * b;
    }

    static float smoothStop(float t, uint32_t power)
    {
        const float t_ = clampTime(t);
        return flip(dumbPow(flip(t_), power));
    }

    static float smoothStart(float t, uint32_t power)
    {
        const float t_ = clampTime(t);
        return dumbPow(t_, power);
    }

    static float smoothStep(float t, uint32_t power)
    {
        return mix(smoothStart(t, power), smoothStop(t, power), t);
    }

    static float smoothStopF(float t, float power)
    {
        const float t_         = clampTime(t);
        const auto  base_power = static_cast<uint32_t>(power);
        return mix(flip(dumbPow(flip(t_), base_power)), flip(dumbPow(flip(t_), base_power + 1)), power - static_cast<float>(base_power));
    }

    static float smoothStartF(float t, float power)
    {
        const float t_         = clampTime(t);
        const auto  base_power = static_cast<uint32_t>(power);
        return mix(dumbPow(t_, base_power), dumbPow(t_, base_power + 1), power - static_cast<float>(base_power));
    }

    static float smoothStepF(float t, float power)
    {
        const float t_ = clampTime(t);
        return mix(smoothStartF(t_, power), smoothStopF(t_, power), t_);
    }

    static float sigmoid(float t, float speed = 1.0f)
    {
        const float s = 20.0f * speed;
        return 1.0f / (1.0f + exp(-(t - 0.5f) * s));
    }

    static float linear(float t, float speed = 1.0f)
    {
        return speed * t;
    }

    static float easeInOut(float t)
    {
        if (t < 0.5f) {
            return std::pow(2.0f, 20.0f * t - 10.0f) * 0.5f;
        }
        return (2.0f - std::pow(2.0f, -20.0f * t + 10.0f)) * 0.5f;
    }

    static float easeInOutCirc(float t)
    {
        if (t < 0.5f) {
            return (1.0f - std::sqrt(1.0f - std::pow(2.0f * t, 2.0f))) * 0.5f;
        }
        return (std::sqrt(1.0f - std::pow(-2.0f * t + 2.0f, 2.0f)) + 1.0f) * 0.5f;
    }

    static float easeOutBack(float t)
    {
        constexpr float c1 = 1.70158f;
        constexpr float c3 = c1 + 1.0f;
        return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
    }

    static float easeInOutQuint(float t)
    {
        if (t < 0.5f) {
            return  16.0f * dumbPow(t, 5);
        }
        return 1.0f - dumbPow(-2.0f * t + 2, 5) * 0.5f;
    }

    static float easeOutElastic(float t)
    {
        const float c4 = Math::ConstantF32::TwoPi / 3.0f;

        if (t == 0.0f) {
            return 0.0f;
        } else if (t >= 1.0f) {
            return 1.0f;
        }

        return std::pow(2.0f, -10.0f * t) * sin((t * 10.0f - 0.75f) * c4) + 1.0f;

    }

    static float getInterpolationValue(float t, Interpolation interpolation)
    {
        switch (interpolation) {
            case Interpolation::None:
                return 1.0f;
            case Interpolation::Linear:
                return t;
            case Interpolation::EaseInOutExponential:
                return Smooth::easeInOut(t);
            case Interpolation::EaseInOutCirc:
                return Smooth::easeInOutCirc(t);
            case Interpolation::EaseInOutQuint:
                return Smooth::easeInOutQuint(t);
            case Interpolation::EaseOutBack:
                return Smooth::easeOutBack(t);
            case Interpolation::EaseOutElastic:
                return Smooth::easeOutElastic(t);
            case Interpolation::Sigmoid:
                return Smooth::sigmoid(t);
        }
        // Default to linear
        return t;
    }
};
