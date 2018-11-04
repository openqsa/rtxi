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

#include "stimulation.h"

#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>

namespace Qsa
{
const std::vector<double> & Stimulation::amplitudes() const
{
        return amplitudes_;
}

void Stimulation::apply()
{
        // Start stimulation
        // (it will stop automatically when finished)
        applying_ = true;
}

double Stimulation::drop_delay() const
{
        return drop_delay_;
}

void Stimulation::evaluate(double t, double & output, int & sync) const
{
        if (!applying_)
        {
                output = rest_level_;
                sync = SYNC_OFF;
                return;
        }
        auto iterations = static_cast<std::size_t>(t / frequencies_.dt());
        if (iterations < computed_output_.size())
        {
                output = computed_output_[iterations];
                sync = computed_sync_[iterations];
        }
        else
        {
                applying_ = false;
                output = rest_level_;
                sync = SYNC_OFF;
        }
}

const Frequencies & Stimulation::frequencies() const
{
        return frequencies_;
}

bool Stimulation::is_applying() const
{
        return applying_;
}

const std::vector<double> & Stimulation::phases() const
{
        return phases_;
}

double Stimulation::rest_level() const
{
        return rest_level_;
}

double Stimulation::step_delay() const
{
        return step_delay_;
}

double Stimulation::step_level() const
{
        return step_level_;
}

std::string Stimulation::to_string() const
{
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<double>::digits10 + 1);
        ss << "Dt (s): " << frequencies_.dt() << std::endl;
        ss << "Duration (s): " << frequencies_.duration() << std::endl;
        ss << "Frequencies (Hz):";
        for (auto fundamental : frequencies_.fundamentals())
                ss << " " << fundamental;
        ss << std::endl;
        ss << "Amplitudes:";
        for (auto amplitude : amplitudes_)
                ss << " " << amplitude;
        ss << std::endl;
        ss << "Phases:";
        for (auto phase : phases_)
                ss << " " << phase;
        ss << std::endl;
        ss << "Rest level: " << rest_level_ << std::endl;
        ss << "Step level: " << step_level_ << std::endl;
        ss << "Step delay (s): " << step_delay_ << std::endl;
        ss << "Trace count: " << trace_count_ << std::endl;
        ss << "Trace pause: " << trace_pause_ << std::endl;
        ss << "Trace alternance: " << trace_alternance_ << std::endl;
        return ss.str();
}

int Stimulation::trace_alternance() const
{
        return trace_alternance_;
}

int Stimulation::trace_count() const
{
        return trace_count_;
}

double Stimulation::trace_pause() const
{
        return trace_pause_;
}

Stimulation::Stimulation(
        const Frequencies & frequencies,
        const std::vector<double> & amplitudes,
        const std::vector<double> & phases,
        double rest_level,
        double step_level,
        double step_delay,
        double drop_delay,
        int trace_count,
        double trace_pause,
        int trace_alternance)
:
        frequencies_(frequencies),
        amplitudes_(amplitudes),
        phases_(phases),
        rest_level_(rest_level),
        step_level_(step_level),
        step_delay_(step_delay),
        drop_delay_(drop_delay),
        trace_count_(trace_count),
        trace_pause_(trace_pause),
        trace_alternance_(trace_alternance),
        applying_(false)
{
        precompute();
}

void Stimulation::precompute()
{
        auto to_ticks = [&](double period)
        {
                return static_cast<std::size_t>(period / frequencies_.dt());
        };
        for (auto i = 0; i < trace_count_; i++)
        {
                // Step (pre)
                auto step_size = to_ticks(step_delay_);
                std::fill_n(
                        std::back_inserter(computed_output_),
                        step_size,
                        step_level_);
                std::fill_n(
                        std::back_inserter(computed_sync_),
                        step_size, SYNC_STEP);

                // Multisine (twice duration)
                auto n = frequencies_.fundamentals().size();
                auto sign = trace_alternance_ < 0 && i % 2 != 0 ? -1 : +1;
                auto multisine_size = to_ticks(2 * frequencies_.duration());
                auto sine_function = [&](double t, int k)
                {
                        auto ak = amplitudes_[k];
                        auto fk = frequencies_.fundamentals()[k];
                        auto pk = phases_[k];
                        return ak * sin(2 * M_PI * fk * t + pk) / n;
                };
                auto multisine_function = [&](double t)
                {
                        auto sum = step_level_;
                        for (auto k = 0U; k < n; k++)
                                sum += sign * sine_function(t, k);
                        return sum;
                };
                for (auto j = 0U; j < multisine_size; j++)
                {
                        auto t = j * frequencies_.dt();
                        auto output = multisine_function(t);
                        auto sync = j < multisine_size / 2 ?
                                SYNC_IGNORE : SYNC_MULTISINE;
                        computed_output_.push_back(output);
                        computed_sync_.push_back(sync);
                }

                // Step (post)
                std::fill_n(
                        std::back_inserter(computed_output_),
                        step_size,
                        step_level_);
                std::fill_n(
                        std::back_inserter(computed_sync_),
                        step_size,
                        SYNC_IGNORE);

                // Drop
                auto drop_size = to_ticks(drop_delay_);
                std::fill_n(
                        std::back_inserter(computed_output_),
                        drop_size,
                        rest_level_);
                std::fill_n(
                        std::back_inserter(computed_sync_),
                        drop_size,
                        SYNC_DROP);

                // Pause
                auto pause_size = to_ticks(trace_pause_);
                std::fill_n(
                        std::back_inserter(computed_output_),
                        pause_size,
                        rest_level_);
                std::fill_n(
                        std::back_inserter(computed_sync_),
                        pause_size,
                        SYNC_IGNORE);
        }
}
}
