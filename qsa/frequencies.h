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

#ifndef QSA_FREQUENCIES_H
#define QSA_FREQUENCIES_H

#include "intermodulation.h"

#include <vector>

namespace Qsa
{
class Frequencies
{
public:
        Frequencies() = default;
        Frequencies(const Frequencies &) = default;
        Frequencies & operator=(const Frequencies &) = default;
        ~Frequencies() = default;

        explicit Frequencies(
                const Intermodulation & intermodulation,
                double dt,
                double duration);

        double dt() const;
        double duration() const;
        const std::vector<double> & fundamentals() const;
        const Intermodulation & intermodulation() const;

private:
        Intermodulation intermodulation_;
        double dt_{};
        double duration_{};
        std::vector<double> fundamentals_;
};
}

#endif /* QSA_FREQUENCIES_H */
