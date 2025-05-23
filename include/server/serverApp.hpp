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
#include "envConfig.hpp"
#include "configLoader.hpp"
#include "mongoDatabase.hpp"
#include "httpManager.hpp"

class ServerApp
{
public:
    ServerApp(EnvConfig& envConfig, ConfigLoader& configLoader);

    ~ServerApp();
    void run();
private:
    EnvConfig& m_envConfig;
    ConfigLoader& m_configLoader;
    MongoDatabase m_mongoDb;
    HttpManager m_httpManager;
};
