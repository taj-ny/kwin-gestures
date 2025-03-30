#pragma once

namespace libgestures
{

template <typename T>
class Range
{
public:
    Range() = default;
    Range(T minmax);
    Range(T min, T max);

    const T &min() const;
    const T &max() const;

    bool contains(const T &value) const;

    template <typename U>
    explicit operator Range<U>() const;

private:
    T m_min{};
    T m_max{};
};

}