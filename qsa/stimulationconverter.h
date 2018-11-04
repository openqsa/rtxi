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

#ifndef QSA_STIMULATIONCONVERTER_H
#define QSA_STIMULATIONCONVERTER_H

#include "stimulation.h"

#include <sstream>
#include <string>

namespace Qsa
{
class StimulationConverter
{
public:
        static Stimulation parse(const std::string & stimulation_string);
        static std::string print(const Stimulation & stimulation);
};
}

#endif /* QSA_STIMULATIONCONVERTER_H */
