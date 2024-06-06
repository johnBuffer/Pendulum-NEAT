#pragma once

template<typename TFloat>
struct Complex
{
    TFloat real      = {};
    TFloat imaginary = {};

    Complex() = default;
    Complex(TFloat x, TFloat y)
        : real{x}
        , imaginary{y}
    {}
};

template<typename TFloat>
Complex<TFloat> operator+(Complex<TFloat> const& c1, Complex<TFloat> const& c2)
{
    return {c1.real + c2.real, c1.imaginary + c2.imaginary};
}

template<typename TFloat>
Complex<TFloat> operator+(TFloat x, Complex<TFloat> const& c)
{
    return {c.real + x, c.imaginary};
}

template<typename TFloat>
Complex<TFloat> operator+(Complex<TFloat> const& c, TFloat x)
{
    return operator+(x, c);
}

// Could optimize if norm is 1
template<typename TFloat>
Complex<TFloat> inverse(Complex<TFloat> const& c)
{
    TFloat inv_norm2 = TFloat{1.0} / (c.real * c.real + c.imaginary * c.imaginary);
    return {c.real * inv_norm2, -c.imaginary * inv_norm2};
}

template<typename TFloat>
Complex<TFloat> normalize(Complex<TFloat> const& c)
{
    TFloat inv_norm = TFloat{1.0} / sqrt(c.real * c.real + c.imaginary * c.imaginary);
    return {c.real * inv_norm, c.imaginary * inv_norm};
}

template<typename TFloat>
Complex<TFloat> operator*(Complex<TFloat> const& c1, Complex<TFloat> const& c2)
{
    return {c1.real * c2.real - c1.imaginary * c2.imaginary, c1.real * c2.imaginary + c1.imaginary * c2.real};
}

template<typename TFloat>
Complex<TFloat> operator*(TFloat x, Complex<TFloat> const& c)
{
    return {x * c.real, x * c.imaginary};
}

template<typename TFloat>
Complex<TFloat> operator*(Complex<TFloat> const& c, TFloat x)
{
    return operator*(x, c);
}

template<typename TFloat>
Complex<TFloat> operator/(TFloat x, Complex<TFloat> const& c)
{
    return x * inverse(c);
}

template<typename TFloat>
Complex<TFloat> operator/(Complex<TFloat> const& c, TFloat x)
{
    return {c.real / x, c.imaginary / x};
}

template<typename TFloat>
Complex<TFloat> operator/(Complex<TFloat> const& c1, Complex<TFloat> const& c2)
{
    return c1 * inverse(c2);
}

template<typename TFloat>
TFloat getArgument(Complex<TFloat> const& c)
{
    return atan2(c.imaginary, c.real);
}


