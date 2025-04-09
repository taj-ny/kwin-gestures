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

    /**
     * @return A threshold that ranges from min to the maximum positive value.
     */
    static Range<T> minToInf(const T &min);
    /**
     * @return A threshold that ranges from the maximum negative value to max.
     */
    static Range<T> infToMax(const T &max);

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