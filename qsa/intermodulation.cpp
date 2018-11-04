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

#include "intermodulation.h"

#include <algorithm>
#include <random>

namespace Qsa
{
std::set<int> mix(const std::set<int> & generators, int k)
{
        // Quadratic frequency mixing between generators and k
        std::set<int> mixing;
        if (mixing.count(k))
                return {};
        mixing.insert(k);
        if (mixing.count(2 * k))
                return {};
        mixing.insert(2 * k);
        for (auto generator : generators)
        {
                if (mixing.count(k + generator))
                        return {};
                mixing.insert(k + generator);
                if (mixing.count(std::abs(k - generator)))
                        return {};
                mixing.insert(std::abs(k - generator));
        }
        return mixing;
}

bool overlap(const std::set<int> & products, const std::set<int> & mixing)
{
        // Detect frequency overlapping before to inject mixing into products
        for (auto product : mixing)
        {
                if (products.count(product))
                        return true;
        }
        return false;
}

std::vector<int> random_range(int a, int b, int seed = 0)
{
        // Create random permutation of range [a .. b]
        std::vector<int> source(b - a + 1);
        std::iota(source.begin(), source.end(), a);
        std::random_device rd;
        std::seed_seq seq{seed == 0 ? rd() : seed};
        std::mt19937 mersenne_engine{seq};
        std::shuffle(source.begin(), source.end(), mersenne_engine);
        return source;
}

Intermodulation Intermodulation::make(const std::vector<int> & source)
{
        // Compute generators and products from a given source
        std::set<int> generators;
        std::set<int> products;
        for (auto k : source)
        {
                // Compute quadratic frequency mixing without overlapping
                std::set<int> mixing = mix(generators, k);
                if (mixing.empty())
                        continue;
                if (overlap(products, mixing))
                        continue;

                // Update generators and products
                generators.insert(k);
                products.insert(mixing.begin(), mixing.end());
        }
        return Intermodulation(generators, products);
}

Intermodulation Intermodulation::make(int a, int b, int seed)
{
        // Generate intermodulation from random source between [a .. b]
        return make(random_range(a, b, seed));
}

const std::set<int> & Intermodulation::generators() const
{
        return generators_;
}

const std::set<int> & Intermodulation::products() const
{
        return products_;
}

Intermodulation::Intermodulation(
        const std::set<int> & generators,
        const std::set<int> & products)
:
        generators_(generators),
        products_(products)
{
}
}
