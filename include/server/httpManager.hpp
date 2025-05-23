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
#include "mongoDatabase.hpp"
#include "httplib.h"    
#include "nlohmann/json.hpp"
#include "bsoncxx/v_noabi/bsoncxx/json.hpp"
#include "maxFlowCalculator.hpp"
#include "circuitCalculator.hpp"
#include <thread>

class HttpManager
{
public:
    HttpManager(const std::string ip, uint16_t port, MongoDatabase& mongoDb);
    ~HttpManager();
    void start();
    void stop();

private:
    static constexpr const char* ENDPOINT_MAP = "/map";
    static constexpr const char* ENDPOINT_MAXFLOW = "fullfilment-flow";
    static constexpr const char* ENDPOINT_CIRCUIT = "/circuit";
    static constexpr const char* ENDPOINT_RESULT = "/result";

    std::string m_ip;
    uint16_t m_port;

    std::thread m_thread;
    bool m_running = false;
    void run();

    httplib::Server m_sv;

    MongoDatabase& m_mongoDb;
    
    MaxFlowCalculator m_maxFlowCalculator;
    CircuitCalculator m_circuitCalculator;
};