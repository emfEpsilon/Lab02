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
#include "serverApp.hpp"

ServerApp::ServerApp(EnvConfig &envConfig, ConfigLoader &configLoader)
try
    : m_envConfig(envConfig),
      m_configLoader(configLoader),
      m_mongoDb(envConfig.getMongoUri(), envConfig.getMongoDbName()),
      m_httpManager(envConfig.getHttpIp(), envConfig.getHttpPort(), m_mongoDb)
{
}
catch (const std::exception &e)
{
  std::cerr << "Error initializing ServerApp: " << e.what() << std::endl;
  throw;
}

ServerApp::~ServerApp()
{
  
}

void ServerApp::run()
{
  m_httpManager.start();
  while (true)
  {
    std::cout << "Server is running..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}