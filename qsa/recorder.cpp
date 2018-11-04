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

#include "recorder.h"

#include "version.h"

#include <nlohmann/json.hpp>

#include <cmath>
#include <fstream>

using json = nlohmann::json;

namespace Qsa
{
Recorder::Recorder()
:
        sync_(Stimulation::SYNC_OFF),
        started_(false),
        stopped_(false)
{
}

void Recorder::push(double in, double out, double sync)
{
        auto epsilon = stimulation_.frequencies().dt() / 2;

        if (abs(sync - Stimulation::SYNC_STEP) < epsilon)
                push_step(in, out);
        else if (abs(sync - Stimulation::SYNC_MULTISINE) < epsilon)
                push_multisine(in, out);
        else if (abs(sync - Stimulation::SYNC_DROP) < epsilon)
                push_drop(in, out);
        else if (abs(sync - Stimulation::SYNC_OFF) < epsilon)
                stop();
        else if (abs(sync - Stimulation::SYNC_IGNORE) < epsilon)
                sync_ = Stimulation::SYNC_IGNORE;
}

void Recorder::save(const std::string & filename) const
{
        if (traces_.empty())
                return;
        json j;
        j["version"] = Qsa::VERSION;
        j["dt"] = stimulation_.frequencies().dt();
        j["duration"] = stimulation_.frequencies().duration();
        j["frequencies"] = stimulation_.frequencies().fundamentals();
        j["amplitudes"] = stimulation_.amplitudes();
        j["phases"] = stimulation_.phases();
        j["rest_level"] = stimulation_.rest_level();
        j["step_level"] = stimulation_.step_level();
        j["step_delay"] = stimulation_.step_delay();
        j["drop_delay"] = stimulation_.drop_delay();
        j["trace_count"] = stimulation_.trace_count();
        j["trace_pause"] = stimulation_.trace_pause();
        j["trace_alternance"] = stimulation_.trace_alternance();
        j["traces"] = json::array();
        for (auto it = traces_.begin(); it != traces_.end(); it++)
        {
                json jtrace;
                jtrace["step"]["time"] = it->step_time_sequence_;
                jtrace["step"]["stimulation"] = it->step_stimulation_sequence_;
                jtrace["step"]["response"] = it->step_response_sequence_;
                //
                jtrace["multisine"]["time"] = it->multisine_time_sequence_;
                jtrace["multisine"]["stimulation"] = it->multisine_stimulation_sequence_;
                jtrace["multisine"]["response"] = it->multisine_response_sequence_;
                //
                jtrace["drop"]["time"] = it->drop_time_sequence_;
                jtrace["drop"]["stimulation"] = it->drop_stimulation_sequence_;
                jtrace["drop"]["response"] = it->drop_response_sequence_;
                j["traces"].push_back(jtrace);
        }
        std::ofstream file;
        file.open(filename);
        file << j;
        file.close();
}

void Recorder::set_stimulation(const Stimulation & stimulation)
{
        stimulation_ = stimulation;
}

void Recorder::start()
{
        traces_ = {};
        cycles_ = 0;
        sync_ = Stimulation::SYNC_OFF;
        started_ = false; // Not really started before first step
        stopped_ = false;
}

bool Recorder::started() const
{
        return started_;
}

const Stimulation & Recorder::stimulation() const
{
        return stimulation_;
}

void Recorder::stop()
{
        cycles_ = 0;
        sync_ = Stimulation::SYNC_OFF;
        if (started_)
                stopped_ = true;
}

bool Recorder::stopped() const
{
        return stopped_;
}

void Recorder::push_step(double in, double out)
{
        if (sync_ != Stimulation::SYNC_STEP)
        {
                // Real start
                started_ = true;
                // Create trace
                traces_.push_back({});
                // Switch to STEP mode
                cycles_ =
                        stimulation_.step_delay()
                        / stimulation_.frequencies().dt();
                sync_ = Stimulation::SYNC_STEP;
        }
        if (cycles_ >= 0)
        {
                // Record time, stimulation, response
                auto elapsed = cycles_ * stimulation_.frequencies().dt();
                auto t = stimulation_.step_delay() - elapsed;
                traces_.back().step_time_sequence_.push_back(t);
                traces_.back().step_stimulation_sequence_.push_back(in);
                traces_.back().step_response_sequence_.push_back(out);
                cycles_--;
        }
}

void Recorder::push_multisine(double in, double out)
{
        if (sync_ != Stimulation::SYNC_MULTISINE)
        {
                // Switch to MULTISINE mode
                cycles_ =
                        stimulation_.frequencies().duration()
                        / stimulation_.frequencies().dt();
                sync_ = Stimulation::SYNC_MULTISINE;
        }
        if (cycles_ >= 0)
        {
                // Record time, stimulation, response
                auto elapsed = cycles_ * stimulation_.frequencies().dt();
                auto t = stimulation_.frequencies().duration() - elapsed;
                traces_.back().multisine_time_sequence_.push_back(t);
                traces_.back().multisine_stimulation_sequence_.push_back(in);
                traces_.back().multisine_response_sequence_.push_back(out);
                cycles_--;
        }
}

void Recorder::push_drop(double in, double out)
{
        if (sync_ != Stimulation::SYNC_DROP)
        {
                // Switch to DROP mode
                cycles_ =
                        stimulation_.drop_delay()
                        / stimulation_.frequencies().dt();
                sync_ = Stimulation::SYNC_DROP;
        }
        if (cycles_ >= 0)
        {
                // Record time, stimulation, response
                auto elapsed = cycles_ * stimulation_.frequencies().dt();
                auto t = stimulation_.drop_delay() - elapsed;
                traces_.back().drop_time_sequence_.push_back(t);
                traces_.back().drop_stimulation_sequence_.push_back(in);
                traces_.back().drop_response_sequence_.push_back(out);
                cycles_--;
        }
}
}
