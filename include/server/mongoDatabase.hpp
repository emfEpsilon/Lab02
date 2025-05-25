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
#include "mongocxx/v_noabi/mongocxx/instance.hpp"
#include "mongocxx/v_noabi/mongocxx/database.hpp"
#include "mongocxx/v_noabi/mongocxx/client.hpp"
#include "bsoncxx/v_noabi/bsoncxx/json.hpp"
#include "bsoncxx/v_noabi/bsoncxx/builder/stream/document.hpp"
#include "mongocxx/v_noabi/mongocxx/exception/exception.hpp"
#include "bsoncxx/v_noabi/bsoncxx/exception/exception.hpp"



class MongoDatabase
{
public:
    MongoDatabase(const std::string &uri, const std::string &dbName);
    ~MongoDatabase();
    std::string insertMap(const bsoncxx::document::value &bsonMap);
    std::string insertMaxFlowResult(const bsoncxx::builder::stream::document &bsonMaxFlowResult);
    std::string insertCircuitResult(const bsoncxx::builder::stream::document &bsonCircuitResult);

    std::shared_ptr<bsoncxx::document::value> getMap(const std::string &mapId);
    std::shared_ptr<bsoncxx::document::value> getResult(const std::string &resultId);

private:
    mongocxx::instance m_instance; // This should be created only once per application
    mongocxx::client m_client;
    std::string m_dbName;
};