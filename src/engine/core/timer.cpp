#include "./timer.hpp"
#include "engine/engine.hpp"


pez::core::Timer::Timer()
{
    reset();
}

float pez::core::Timer::getElapsedTime() const
{
    return pez::core::getTime() - m_start_time;
}

void pez::core::Timer::reset()
{
    m_start_time = pez::core::getTime();
}

bool pez::core::Timer::isReady(float target_time) const
{
    return getElapsedTime() >= target_time;
}
