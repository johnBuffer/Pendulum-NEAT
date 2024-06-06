#pragma once
#include <vector>

#include "engine/common/index_vector.hpp"

namespace pbd
{
struct Constraint
{
    RealType lambda     = static_cast<RealType>(0.0);
    RealType compliance = static_cast<RealType>(0.0001);
    RealType force      = static_cast<RealType>(0.0);
};
}