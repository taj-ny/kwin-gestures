/*
    Input Actions - Input handler that executes user-defined actions
    Copyright (C) 2024-2025 Marcin Woźniak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "range.h"

#include <limits>

#include <QPointF>

namespace libinputactions
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
Range<T> Range<T>::minToInf(const T &min)
{
    return Range(min, std::numeric_limits<T>::max());
}

template <typename T>
Range<T> Range<T>::infToMax(const T &max)
{
    return Range(std::numeric_limits<T>::lowest(), max);
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