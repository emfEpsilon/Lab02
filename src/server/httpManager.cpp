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
    m_sv.Post(ENDPOINT_MAP, [this](const httplib::Request &req, httplib::Response &res)
              { this->handleMap(req, res); });

    m_sv.Post(ENDPOINT_MAXFLOW, [this](const httplib::Request &req, httplib::Response &res)
              { this->handleMaxFlow(req, res); });

    m_sv.Post(ENDPOINT_CIRCUIT, [this](const httplib::Request &req, httplib::Response &res)
              { this->handleCircuit(req, res); });

    m_sv.Get(ENDPOINT_RESULT, [this](const httplib::Request &req, httplib::Response &res)
             { this->handleResult(req, res); });
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
    try
    {
        m_sv.stop();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error stopping server: " << e.what() << std::endl;
    }
}

void HttpManager::handleMap(const httplib::Request &req, httplib::Response &res)
{
    try
    {
        // 1. Check if proper http

        // 2. Check if json is valid
        bsoncxx::document::value bsonBody = bsoncxx::from_json(req.body);

        // 3. Authenticate

        // 4. Insert map into MongoDB
        std::string mapId = m_mongoDb.insertMap(bsonBody);

        // 5. Response with the map id as json
        res.status = 200;
        res.set_content("{\"map_id\": \"" + mapId + "\"}", "application/json");
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid Access\"}", "application/json");
        return;
    }
}

void HttpManager::handleMaxFlow(const httplib::Request &req, httplib::Response &res)
{
    try
    {
        // 1. Check if proper http

        // 2. Check if json is valid
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);

        // 3. Authenticate

        // 4. Check if map_id is valid

        // 5. Check if source and sink are valid

        // 6. Load map from MongoDB
        std::shared_ptr<bsoncxx::document::value> mapBson = m_mongoDb.getMap(jsonBody["map_id"]);
        if (!mapBson)
        {
            throw std::runtime_error("Map not found");
        }

        // 7. Traverse Map and load calculator
        MaxFlowCalculator maxFlowCalculator;
        std::map<std::string, int> nodeIdMapper; // Map string node IDs to int indices
        int mapperCounter = 0;
        for (bsoncxx::array::element &nodeArrElem : (*mapBson).view()["data"].get_array().value)
        {
            std::string nodeIdFrom = nodeArrElem.get_document().value["node_id"].get_string().value.to_string();
            std::string nodeType = nodeArrElem.get_document().value["node_type"].get_string().value.to_string();

            // TODO: Filter by active nodes

            if (nodeIdMapper.find(nodeIdFrom) == nodeIdMapper.end())
            {
                nodeIdMapper[nodeIdFrom] = mapperCounter++;
            }

            if (nodeType == "fulfillment_center")
            {
                for (bsoncxx::array::element &connArrElem : nodeArrElem.get_document().value["connections"].get_array().value)
                {
                    std::string nodeIdTo = connArrElem.get_document().value["to"].get_string().value.to_string();

                    if (nodeIdMapper.find(nodeIdTo) == nodeIdMapper.end())
                    {
                        nodeIdMapper[nodeIdTo] = mapperCounter++;
                    }
                    maxFlowCalculator.addEdge(nodeIdMapper[nodeIdFrom], nodeIdMapper[nodeIdTo], -1);
                }
            }
        }

        // 8. Calculate Ford-Fulkerson
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        int maxFlow = maxFlowCalculator.calculate(nodeIdMapper[jsonBody["source"]], nodeIdMapper[jsonBody["sink"]]);
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> dulation = end - start;

        // 9. Prepare result json to store
        bsoncxx::builder::stream::document bsonMaxFlowResult;
        bsonMaxFlowResult
            << "source" << jsonBody["source"].get<std::string>()
            << "sink" << jsonBody["sink"].get<std::string>()
            << "max_flow" << maxFlow
            << "duration_ms" << dulation.count()
            << "timestamp" << bsoncxx::types::b_date(std::chrono::system_clock::now())
            << "map_id" << jsonBody["map_id"].get<std::string>();

        // 10. Store result in MongoDB and return id
        std::string resultId = m_mongoDb.insertMaxFlowResult(bsonMaxFlowResult);

        // 11. Response with the result id as json
        res.status = 200;
        res.set_content("{\"result_id\": \"" + resultId + "\"}", "application/json");
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid access\"}", "application/json");
        return;
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid access\"}", "application/json");
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("{\"error\": \"Internal Server Error\"}", "application/json");
        return;
    }
}

void HttpManager::handleCircuit(const httplib::Request &req, httplib::Response &res)
{
    try
    {
        // 1. Check if proper http
        // 2. Check if json is valid
        nlohmann::json jsonBody = nlohmann::json::parse(req.body);
        // 3. Authenticate
        // 4. Check if map_id is valid
        // 5. Check if source and sink are valid
        // 6. Load map from MongoDB
        std::shared_ptr<bsoncxx::document::value> mapBson = m_mongoDb.getMap(jsonBody["map_id"]);
        if (!mapBson)
        {
            res.status = 404;
            res.set_content("{\"error\": \"Map not found\"}", "application/json");
            return;
        }
        // 7. Traverse Map and load calculator
        CircuitCalculator circuitCalculator;
        std::map<std::string, int> nodeIdMapper; // Map string node IDs to int indices
        int mapperCounter = 0;
        for (bsoncxx::array::element &nodeArrElem : (*mapBson).view()["data"].get_array().value)
        {
            std::string nodeIdFrom = nodeArrElem.get_document().value["node_id"].get_string().value.to_string();
            std::string nodeType = nodeArrElem.get_document().value["node_type"].get_string().value.to_string();

            // TODO: Filter by active nodes

            if (nodeIdMapper.find(nodeIdFrom) == nodeIdMapper.end())
            {
                nodeIdMapper[nodeIdFrom] = mapperCounter++;
            }

            if (nodeType == "fulfillment_center")
            {
                for (bsoncxx::array::element &connArrElem : nodeArrElem.get_document().value["connections"].get_array().value)
                {
                    std::string nodeIdTo = connArrElem.get_document().value["to"].get_string().value.to_string();

                    if (nodeIdMapper.find(nodeIdTo) == nodeIdMapper.end())
                    {
                        nodeIdMapper[nodeIdTo] = mapperCounter++;
                    }
                    circuitCalculator.addEdge(nodeIdMapper[nodeIdFrom], nodeIdMapper[nodeIdTo]);
                }
            }
        }
        // 8. Calculate Circuit using Kaufman method (Kaufman-Malgrange algorithm)
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        std::vector<int> circuit = circuitCalculator.calculate(nodeIdMapper[jsonBody["start"]]);
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> dulation = end - start;

        // Convert circuit from int to string to append in bsonCircuitResult
        /*
        std::vector<std::string> circuitString;
        for (int nodeId : circuit)
        {
            // Find the original string ID from the mapper
            auto it = std::find_if(nodeIdMapper.begin(), nodeIdMapper.end(),
                                   [nodeId](const std::pair<std::string, int> &pair)
                                   { return pair.second == nodeId; });
            if (it != nodeIdMapper.end())
            {
                circuitString.push_back(it->first);
            }
        }*/

        // 9. Prepare result json to store
        bsoncxx::builder::stream::document bsonCircuitResult;
        bsonCircuitResult
            << "begin" << jsonBody["start"].get<std::string>()
            << "duration_ms" << dulation.count()
            << "timestamp" << bsoncxx::types::b_date(std::chrono::system_clock::now())
            << "map_id" << jsonBody["map_id"].get<std::string>();

        std::cout << "bsonCircuitResult: " << bsoncxx::to_json(bsonCircuitResult.view()) << std::endl;

        // 10. Store result in MongoDB and return id
        std::string resultId = m_mongoDb.insertCircuitResult(bsonCircuitResult);

        // 11. Response with the result id as json
        res.status = 200;
        res.set_content("{\"result_id\": \"" + resultId + "\"}", "application/json");
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid access\"}", "application/json");
        return;
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid access\"}", "application/json");
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("{\"error\": \"Internal Server Error\"}", "application/json");
        return;
    }
}

void HttpManager::handleResult(const httplib::Request &req, httplib::Response &res)
{
    try
    {
        // 1. Check if proper http

        // 2. Check if proper paramters
        std::string resultId = req.get_param_value("result_id");
        std::cout << "Result ID: " << resultId << std::endl;

        // 3. Authenticate

        // 4. Load map from MongoDB
        std::shared_ptr<bsoncxx::document::value> resultBson = m_mongoDb.getResult(resultId);
        if (!resultBson)
        {
            res.status = 404;
            res.set_content("{\"error\": \"Result not found\"}", "application/json");
            return;
        }

        // 5. Response with the result json
        res.status = 200;
        res.set_content(bsoncxx::to_json((*resultBson).view()), "application/json");
    }
    catch (const bsoncxx::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 400;
        res.set_content("{\"error\": \"Invalid access\"}", "application/json");
        return;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        res.status = 500;
        res.set_content("{\"error\": \"Internal Server Error\"}", "application/json");
        return;
    }
}
