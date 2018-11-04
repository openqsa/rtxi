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

#ifndef QSA_STIMULATIONBUILDER_H
#define QSA_STIMULATIONBUILDER_H

#include "stimulation.h"

#include <vector>

namespace Qsa
{
class StimulationBuilder
{
public:
        StimulationBuilder();

        Stimulation build() const;
        StimulationBuilder & set_amplitude(double amplitude);
        StimulationBuilder & set_dt(double dt);
        StimulationBuilder & set_duration(double duration);
        StimulationBuilder & set_min_frequency(double min_frequency);
        StimulationBuilder & set_max_frequency(double max_frequency);
        StimulationBuilder & set_rest_level(double rest_level);
        StimulationBuilder & set_seed_frequencies(int seed_frequencies);
        StimulationBuilder & set_seed_phases(int seed_phases);
        StimulationBuilder & set_step_delay(double step_delay);
        StimulationBuilder & set_step_level(double step_level);
        StimulationBuilder & set_drop_delay(double drop_delay);
        StimulationBuilder & set_trace_count(int trace_count);
        StimulationBuilder & set_trace_pause(double trace_pause);
        StimulationBuilder & set_trace_alternance(int trace_alternance);

private:
        std::vector<double> build_phases(std::size_t n) const;

        double dt_;
        double duration_;
        double min_frequency_;
        double max_frequency_;
        double amplitude_;
        int seed_frequencies_;
        int seed_phases_;
        double rest_level_;
        double step_level_;
        double step_delay_;
        double drop_delay_;
        int trace_count_;
        double trace_pause_;
        int trace_alternance_;
};
}

#endif /* QSA_STIMULATIONBUILDER_H */
