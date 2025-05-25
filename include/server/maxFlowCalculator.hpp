/*
 * Lab01Logistics
 * Copyright (C) 2025  Deadlock-Dragons
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include "utils.hpp"
#include <vector>

/**
 * @class MaxFlowCalculator
 * @brief Class to calculate the maximum flow using the Ford-Fulkerson algorithm
 */
class MaxFlowCalculator
{
public:
    MaxFlowCalculator();
    ~MaxFlowCalculator();
    void addEdge(int u, int v, int capacity);
    /**
     * @brief Ford-Fulkerson algorithm to find the maximum flow in a flow network
     * @param s Source node
     * @param k Sink node
     */
    int calculate(int s, int k);

private:
    std::vector<std::vector<int>> m_M;
};