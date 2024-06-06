#pragma once

namespace pbd
{

template<typename TFloat>
struct Matrix2D
{
    TFloat data[9];

    Matrix2D()
    {
        for (auto& v : data) {
            v = 0.0;
        }
        for (int32_t i{0}; i<3; ++i) {
            get(i, i) = 1.0;
        }
    }

    TFloat& get(int32_t row, int32_t col)
    {
        return data[col * 9 + row];
    }
};

}