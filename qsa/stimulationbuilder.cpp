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

#include "stimulationbuilder.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace Qsa
{
StimulationBuilder::StimulationBuilder()
:
        dt_(0.0),
        duration_(1.0),
        min_frequency_(1.0),
        max_frequency_(1.0),
        amplitude_(1.0),
        seed_frequencies_(0),
        seed_phases_(0),
        rest_level_(0.0),
        step_level_(0.0),
        step_delay_(0.0),
        drop_delay_(1.0),
        trace_count_(1),
        trace_pause_(0.0),
        trace_alternance_(1)
{
}

Stimulation StimulationBuilder::build() const
{
        auto df = 1 / duration_;
        auto intermodulation = Qsa::Intermodulation::make(
                min_frequency_ / df,
                max_frequency_ / df,
                seed_frequencies_);
        auto n = intermodulation.generators().size();
        Qsa::Frequencies frequencies{intermodulation, dt_, duration_};
        std::vector<double> amplitudes(n, amplitude_);
        auto phases = build_phases(n);
        return Stimulation{
                frequencies,
                amplitudes,
                phases,
                rest_level_,
                step_level_,
                step_delay_,
                drop_delay_,
                trace_count_,
                trace_pause_,
                trace_alternance_};
}

std::vector<double> StimulationBuilder::build_phases(std::size_t n) const
{
        std::random_device rd;
        std::seed_seq seq{seed_phases_ == 0 ? rd() : seed_phases_};
        std::mt19937 mersenne_engine{seq};
        std::uniform_real_distribution<double> distribution{0, 2 * M_PI};
        auto generate = [&distribution, &mersenne_engine]()
        {
                return distribution(mersenne_engine);
        };
        std::vector<double> phases(n);
        std::generate(begin(phases), end(phases), generate);
        return phases;
}

StimulationBuilder & StimulationBuilder::set_amplitude(double amplitude)
{
        amplitude_ = amplitude;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_dt(double dt)
{
        dt_ = dt;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_duration(double duration)
{
        duration_ = duration;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_min_frequency(double min_frequency)
{
        min_frequency_ = min_frequency;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_max_frequency(double max_frequency)
{
        max_frequency_ = max_frequency;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_rest_level(double rest_level)
{
        rest_level_ = rest_level;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_seed_frequencies(
        int seed_frequencies)
{
        seed_frequencies_ = seed_frequencies;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_seed_phases(int seed_phases)
{
        seed_phases_ = seed_phases;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_step_delay(double step_delay)
{
        step_delay_ = step_delay;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_step_level(double step_level)
{
        step_level_ = step_level;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_drop_delay(double drop_delay)
{
        drop_delay_ = drop_delay;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_trace_count(int trace_count)
{
        trace_count_ = trace_count;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_trace_pause(double trace_pause)
{
        trace_pause_ = trace_pause;
        return *this;
}

StimulationBuilder & StimulationBuilder::set_trace_alternance(
        int trace_alternance)
{
        trace_alternance_ = trace_alternance;
        return *this;
}
}
