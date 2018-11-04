/*
 * Quadratic Sinusoidal Analysis.
 * Copyright (C) 2018 OpenQSA.
 * 
 * This file is part of OpenQSA.
 * 
 * OpenQSA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * OpenQSA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with OpenQSA.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef QSA_INTERMODULATION_H
#define QSA_INTERMODULATION_H

#include <set>
#include <vector>

namespace Qsa
{
class Intermodulation
{
public:
        Intermodulation() = default;
        Intermodulation(const Intermodulation &) = default;
        Intermodulation & operator=(const Intermodulation &) = default;
        ~Intermodulation() = default;

        static Intermodulation make(const std::vector<int> & source);
        static Intermodulation make(int a, int b, int seed = 0);

        const std::set<int> & generators() const;
        const std::set<int> & products() const;

private:
        explicit Intermodulation(
                const std::set<int> & generators,
                const std::set<int> & products);

        std::set<int> generators_;
        std::set<int> products_;
};
}

#endif /* QSA_INTERMODULATION_H */
