#pragma once

namespace pez::core
{
class Timer
{
public:
    Timer();

    [[nodiscard]]
    float getElapsedTime() const;

    void reset();

    [[nodiscard]]
    bool isReady(float target_time) const;

private:
    float m_start_time = 0.0f;
};

}
