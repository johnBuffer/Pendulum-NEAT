#pragma once
#include "index_vector.hpp"
#include <sstream>
#include <iomanip>


template<typename U, typename T>
U to(const T& v)
{
    return static_cast<U>(v);
}


template<typename T>
using CIVector = siv::IndexVector<T>;


template<typename T>
T sign(T v)
{
    return v < 0.0f ? -1.0f : 1.0f;
}


template<typename T>
std::string toString(const T& v, const uint8_t decimals = 2)
{
    std::stringstream sx;
    sx << std::setprecision(decimals) << std::fixed << v;
    return sx.str();
}


template<template<typename> class ContainerType, typename T, typename TCallback>
void enumerate(ContainerType<T>& container, TCallback&& callback)
{
    uint64_t i(0);
    for (T& obj : container) {
        callback(i++, obj);
    }
}

template<typename TContainer, typename TPredicate>
void remove_if(TContainer& container, TPredicate&& pred)
{
    container.erase(std::remove_if(container.begin(), container.end(), pred), container.end());
}


template<typename T>
uint64_t getVectorByteSize(const std::vector<T>& v)
{
    return v.size() * sizeof(T);
}

template<typename TMax, typename TCallback, typename TData>
TMax getMax(std::vector<TData> const& v, TCallback&& callback)
{
    // Only works for positive values
    TMax res{0};
    for (auto const& o : v) {
        res = std::max(callback(o), res);
    }
    return res;
}
