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

#ifndef QSA_STIMULATION_H
#define QSA_STIMULATION_H

#include "frequencies.h"

#include <string>
#include <vector>

namespace Qsa
{
class Stimulation
{
public:
        enum Sync
        {
                SYNC_OFF = -1,
                SYNC_IGNORE = 0,
                SYNC_STEP = 2,
                SYNC_MULTISINE = 3,
                SYNC_DROP = 4
        };

        Stimulation() = default;
        Stimulation(const Stimulation &) = default;
        Stimulation & operator=(const Stimulation &) = default;
        ~Stimulation() = default;

        const std::vector<double> & amplitudes() const;
        void apply();
        double drop_delay() const;
        void evaluate(double t, double & output, int & sync) const;
        const Frequencies & frequencies() const;
        bool is_applying() const;
        const std::vector<double> & phases() const;
        double rest_level() const;
        double step_delay() const;
        double step_level() const;
        std::string to_string() const;
        int trace_alternance() const;
        int trace_count() const;
        double trace_pause() const;

private:
        explicit Stimulation(
                const Frequencies & frequencies,
                const std::vector<double> & amplitudes,
                const std::vector<double> & phases,
                double rest_level,
                double step_level,
                double step_delay,
                double drop_delay,
                int trace_count,
                double trace_pause,
                int trace_alternance);

        void precompute();

        Frequencies frequencies_;
        std::vector<double> amplitudes_;
        std::vector<double> phases_;
        double rest_level_;
        double step_level_;
        double step_delay_;
        double drop_delay_;
        int trace_count_;
        double trace_pause_;
        int trace_alternance_;

        mutable bool applying_;
        mutable std::vector<double> computed_output_;
        mutable std::vector<int> computed_sync_;

        friend class StimulationBuilder;
        friend class StimulationConverter;
};
}

#endif /* QSA_STIMULATION_H */
