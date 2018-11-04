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

#include "frequencies.h"

namespace Qsa
{
Frequencies::Frequencies(
        const Intermodulation & intermodulation,
        double dt,
        double duration)
:
        intermodulation_(intermodulation),
        dt_(dt),
        duration_(duration)
{
        for (auto k : intermodulation_.generators())
                fundamentals_.push_back(k / duration_); // in hertz
}

double Frequencies::dt() const
{
        return dt_;
}

double Frequencies::duration() const
{
        return duration_;
}

const std::vector<double> & Frequencies::fundamentals() const
{
        return fundamentals_;
}

const Intermodulation & Frequencies::intermodulation() const
{
        return intermodulation_;
}
}
