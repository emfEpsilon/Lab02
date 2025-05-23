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
#include "httpManager.hpp"

HttpManager::HttpManager(const std::string ip, uint16_t port, MongoDatabase &mongoDb)
try : m_ip(ip),
      m_port(port),
      m_mongoDb(mongoDb)
{
    // m_sv.Post(ENDPOINT_FLOW, [this](const httplib::Request& req, httplib::Response& res) {
    //     this->handleFlow(req, res);
    // });

    m_sv.Post(ENDPOINT_MAP, [this](const httplib::Request &req, httplib::Response &res)
              {
                  bsoncxx::document::value bsonBody = bsoncxx::from_json(req.body);

                  // Print the size of the body in byte
                  std::cout << "Body size: " << req.body.size() << std::endl;

                  // Print the size of bsonBody in bytes
                  std::cout << "Bson size: " << bsonBody.view().length() << std::endl;

                  std::string id = m_mongoDb.insertMap(bsonBody);

                  std::cout << "Inserted map with id: " << id << std::endl;

                  // Response Ok
                  res.status = 200;
                  res.set_content("OK", "text/plain");
                  //
              });

    m_sv.Post(ENDPOINT_MAXFLOW, [this](const httplib::Request &req, httplib::Response &res)
              {
                  // Get map given id from MongoDB
                  nlohmann::json jsonBody = nlohmann::json::parse(req.body);
                  std::shared_ptr<bsoncxx::document::value> mapBson = m_mongoDb.getMap(jsonBody["map_id"]);
                  if (!mapBson)
                  {
                      res.status = 404;
                      res.set_content("Map not found", "text/plain");
                      return;
                  }
                  // Print mapBson
                    std::cout << "Map: " << bsoncxx::to_json(*mapBson) << std::endl;

                  // Calculate Max Flow Path
                  // for each
                  // m_maxFlowCalculator.loadEdge()

                  // std::vector<int> maxFlowPath = m_maxFlowCalculator.calculateMaxFlow(jsonBody["source"], jsonBody["sink"]);

                  // Store result in MongoDB and return id
                  // m_mongoDb.insertMaxFlowPath(maxFlowPath);

                  // Response with the result id
              });

    //  m_sv.Post(ENDPOINT_CIRCUIT, handleCircuit);
    //  m_sv.Get(ENDPOINT_RESULT, handleResult);
}
catch (...)
{
    std::cerr << "Error initializing HttpManager" << std::endl;
}

HttpManager::~HttpManager()
{
}

void HttpManager::start()
{
    m_thread = std::thread(&HttpManager::run, this);
}

void HttpManager::run()
{
    m_sv.listen(m_ip, m_port);
}

void HttpManager::stop()
{
}
