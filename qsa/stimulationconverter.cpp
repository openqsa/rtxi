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

#include "stimulationconverter.h"

#include <iomanip>
#include <limits>

namespace
{
double string_to_double(const std::string & s)
{
        std::istringstream iss(s);
        iss.imbue(std::locale("C"));
        double value;
        iss >> value;
        return value;
}

void parse_line(
        std::stringstream & ss,
        const std::string & key,
        std::string & value)
{
        std::string line;
        if (!std::getline(ss, line))
                throw std::exception();
        auto index = line.find(":");
        if (line.substr(0, index) != key)
                throw std::exception();
        value = line.substr(index + 2);
}

void parse_double(const std::string & value, double & result)
{
        result = string_to_double(value);
}

void parse_int(const std::string & value, int & result)
{
        result = std::stoi(value);
}

void parse_double_vector(
        const std::string & value,
        std::vector<double> & result)
{
        std::stringstream ss(value);
        std::string token;
        result.clear();
        while (std::getline(ss, token, ' '))
        {
                
                if (!token.empty())
                        result.push_back(string_to_double(token));
        }
}

void parse_int_vector(const std::string & value, std::vector<int> & result)
{
        std::stringstream ss(value);
        std::string token;
        result.clear();
        while (std::getline(ss, token, ' '))
        {
                if (!token.empty())
                        result.push_back(std::stoi(token));
        }
}
}

namespace Qsa
{
Stimulation StimulationConverter::parse(const std::string & stimulation_string)
{
        std::stringstream ss(stimulation_string);
        std::string value;
        double dt;
        double duration;
        std::vector<int> generators;
        std::vector<double> amplitudes;
        std::vector<double> phases;
        double rest_level;
        double step_level;
        double step_delay;
        double drop_delay;
        int trace_count;
        double trace_pause;
        int trace_alternance;
        try
        {
                parse_line(ss, "dt", value);
                parse_double(value, dt);
                parse_line(ss, "duration", value);
                parse_double(value, duration);
                parse_line(ss, "generators", value);
                parse_int_vector(value, generators);
                parse_line(ss, "amplitudes", value);
                parse_double_vector(value, amplitudes);
                parse_line(ss, "phases", value);
                parse_double_vector(value, phases);
                parse_line(ss, "rest_level", value);
                parse_double(value, rest_level);
                parse_line(ss, "step_level", value);
                parse_double(value, step_level);
                parse_line(ss, "step_delay", value);
                parse_double(value, step_delay);
                parse_line(ss, "drop_delay", value);
                parse_double(value, drop_delay);
                parse_line(ss, "trace_count", value);
                parse_int(value, trace_count);
                parse_line(ss, "trace_pause", value);
                parse_double(value, trace_pause);
                parse_line(ss, "trace_alternance", value);
                parse_int(value, trace_alternance);
        }
        catch (...)
        {
                return {};
        }
        auto intermodulation = Intermodulation::make(generators);
        Frequencies frequencies{intermodulation, dt, duration};
        return Stimulation{
                frequencies,
                amplitudes,
                phases,
                rest_level,
                step_level,
                step_delay,
                drop_delay,
                trace_count,
                trace_pause,
                trace_alternance};
}

std::string StimulationConverter::print(const Stimulation & stimulation)
{
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<double>::digits10 + 1);
        ss << "dt: " << stimulation.frequencies().dt() << std::endl;
        ss << "duration: " << stimulation.frequencies().duration() << std::endl;
        ss << "generators:";
        for (auto g : stimulation.frequencies().intermodulation().generators())
                ss << " " << g;
        ss << std::endl;
        ss << "amplitudes:";
        for (auto amplitude : stimulation.amplitudes())
                ss << " " << amplitude;
        ss << std::endl;
        ss << "phases:";
        for (auto phase : stimulation.phases())
                ss << " " << phase;
        ss << std::endl;
        ss << "rest_level: " << stimulation.rest_level() << std::endl;
        ss << "step_level: " << stimulation.step_level() << std::endl;
        ss << "step_delay: " << stimulation.step_delay() << std::endl;
        ss << "drop_delay: " << stimulation.drop_delay() << std::endl;
        ss << "trace_count: " << stimulation.trace_count() << std::endl;
        ss << "trace_pause: " << stimulation.trace_pause() << std::endl;
        ss << "trace_alternance: " << stimulation.trace_alternance()
                << std::endl;
        return ss.str();
}
}
