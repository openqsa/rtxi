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

#ifndef QSA_RECORDER_H
#define QSA_RECORDER_H

#include "stimulation.h"

#include <vector>

namespace Qsa
{
class Recorder
{
public:
        Recorder();
        Recorder(const Recorder &) = delete;
        Recorder & operator=(const Recorder &) = delete;
        ~Recorder() = default;

        void push(double in, double out, double sync);
        void save(const std::string & filename) const;
        void set_stimulation(const Stimulation & stimulation);
        void start();
        bool started() const;
        const Stimulation & stimulation() const;
        void stop();
        bool stopped() const;

private:
        struct Trace
        {
                std::vector<double> step_time_sequence_;
                std::vector<double> step_stimulation_sequence_;
                std::vector<double> step_response_sequence_;
                //
                std::vector<double> multisine_time_sequence_;
                std::vector<double> multisine_stimulation_sequence_;
                std::vector<double> multisine_response_sequence_;
                //
                std::vector<double> drop_time_sequence_;
                std::vector<double> drop_stimulation_sequence_;
                std::vector<double> drop_response_sequence_;
        };

        void push_step(double in, double out);
        void push_multisine(double in, double out);
        void push_drop(double in, double out);

        Stimulation::Sync sync_;
        bool started_;
        bool stopped_;
        Stimulation stimulation_;
        long cycles_;
        std::vector<Trace> traces_;
};
}

#endif /* QSA_RECORDER_H */
