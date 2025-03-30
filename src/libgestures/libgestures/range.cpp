#include "range.h"

#include <QPointF>

namespace libgestures
{

template <typename T>
Range<T>::Range(T minmax)
    : Range(minmax, minmax)
{
}

template <typename T>
Range<T>::Range(T min, T max)
    : m_min(min)
    , m_max(max)
{
}

template <typename T>
bool Range<T>::contains(const T &value) const
{
    return value >= m_min && value <= m_max;
}

template <>
bool Range<QPointF>::contains(const QPointF &value) const
{
    return value.x() >= m_min.x() && value.y() >= m_min.y()
        && value.x() <= m_max.x() && value.y() <= m_max.y();
}

template <typename T>
template <typename U>
Range<T>::operator Range<U>() const
{
    return Range<U>(static_cast<U>(min), static_cast<U>(max));
}

template <typename T>
const T &Range<T>::min() const
{
    return m_min;
}

template <typename T>
const T &Range<T>::max() const
{
    return m_max;
}

template class Range<uint8_t>;
template class Range<qreal>;
template class Range<QPointF>;

}