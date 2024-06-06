#pragma once
#include <cmath>
#include "common_configuration.hpp"


namespace nt
{

using ActivationPtr = conf::RealType (*)(conf::RealType);

enum class Activation : uint8_t
{
    None,
    Sigm,
    Relu,
    Tanh,
};

struct ActivationFunction
{
    static ActivationPtr getFunction(Activation activation)
    {
        switch (activation) {
            case Activation::None:
                return none;
            case Activation::Sigm:
                return sigm;
            case Activation::Relu:
                return relu;
            case Activation::Tanh:
                return tanh;
            default:
                return none;
        }
    }

    static conf::RealType none(conf::RealType x)
    {
        return x;
    }

    static conf::RealType sigm(conf::RealType x)
    {
        return 1.0f / (1.0f + std::exp(-4.9 * x));
    }

    static conf::RealType relu(conf::RealType x)
    {
        return (x + std::abs(x)) * 0.5;
    }

    static conf::RealType tanh(conf::RealType x)
    {
        return std::tanh(x);
    }
};

}