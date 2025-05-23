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
#include "envConfig.hpp"

EnvConfig::EnvConfig()
{
    m_mongoUri = "mongodb://172.21.0.2:27017";
    m_mongoDbName = "testdb";
    m_httpIp = "0.0.0.0";
    m_httpPort = 8080;
}
EnvConfig::~EnvConfig()
{
}

std::string EnvConfig::getMongoUri() const
{
    return m_mongoUri;
}
std::string EnvConfig::getMongoDbName() const
{
    return m_mongoDbName;
}
std::string EnvConfig::getHttpIp() const
{
    return m_httpIp;
}

uint16_t EnvConfig::getHttpPort() const
{
    return m_httpPort;
}
