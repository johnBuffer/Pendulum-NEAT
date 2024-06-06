#pragma once
#include "smooth.hpp"
#include "engine/engine.hpp"


template<typename TValueType>
struct SmoothValue
{
    static constexpr float time_margin = 0.0f;

    Interpolation interpolation_function = Interpolation::EaseInOutExponential;

    // Data attributes
    float    start_time      = 0.0f;
    uint32_t smooth_strength = 8;

    TValueType start_value  = {};
    TValueType target_value = {};

private:
    float m_speed = 1.0f;

public:
    SmoothValue() = default;

    explicit
    SmoothValue(const TValueType& value)
    {
        setValueInstant(value);
    }

    SmoothValue(const TValueType& value, float speed)
        : start_value{value}
        , m_speed{speed}
    {

    }

    void setSpeed(float speed)
    {
        /*const float now = pez::core::getTime();
        start_time = now - (now - start_time) * (speed / speed_);*/
        m_speed = speed;
    }

    [[nodiscard]]
    TValueType get() const
    {
        return getCurrentValue();
    }

    void setValueInstant(const TValueType& value)
    {
        start_value  = value;
        target_value = value;
        updateStartTime();
    }

    SmoothValue& operator=(const TValueType& new_value)
    {
        setValue(new_value);
        return *this;
    }

    void operator+=(const TValueType& value)
    {
        this->operator=(target_value + value);
    }

    [[nodiscard]]
    operator TValueType() const
    {
        return getCurrentValue();
    }

    [[nodiscard]]
    float getElapsedTime() const
    {
        return (pez::core::getTime() - start_time) * m_speed;
    }

    [[nodiscard]]
    float getCurrentT() const
    {
        const float t = getElapsedTime();
        return Smooth::getInterpolationValue(t, interpolation_function);
    }

    [[nodiscard]]
    TValueType getCurrentValue() const
    {
        const float t = getCurrentT();
        if (!isDone()) {
            const float inv_ratio = 1.0f - t;
            return start_value * inv_ratio + target_value * t;
        }
        return target_value;
    }

    [[nodiscard]]
    bool isDone() const
    {
        return getElapsedTime() > (1.0f + time_margin);
    }

    void updateStartTime()
    {
        start_time = pez::core::getTime();
    }

    [[nodiscard]]
    const TValueType& getTargetValue() const
    {
        return target_value;
    }

    void setValue(const TValueType& new_value, float speed, Interpolation interpolation)
    {
        start_value  = getCurrentValue();
        target_value = new_value;
        updateStartTime();
        // Update settings
        m_speed                = speed;
        interpolation_function = interpolation;
    }

    void setInterpolationFunction(Interpolation interpolation)
    {
        interpolation_function = interpolation;
    }

    void setValue(const TValueType& new_value)
    {
        start_value  = getCurrentValue();
        target_value = new_value;
        updateStartTime();
    }
};

using SmoothFloat = SmoothValue<float>;
using SmoothVec2  = SmoothValue<Vec2>;
using SmoothVec3  = SmoothValue<Vec3>;
