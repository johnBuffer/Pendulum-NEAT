#pragma once

template<typename T>
struct DoubleObject
{
    T objects[2];
    uint32_t current = 0;

    template<typename... TArgs>
    DoubleObject(TArgs&&... args)
    {
        objects[0] = T{std::forward<TArgs>(args)...};
        objects[1] = T{std::forward<TArgs>(args)...};
    }

    T& getFront()
    {
        return objects[current];
    }

    T& getBack()
    {
        return objects[!current];
    }

    void swap()
    {
        current = !current;
    }
};