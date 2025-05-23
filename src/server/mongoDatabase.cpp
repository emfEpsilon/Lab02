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
#include "mongoDatabase.hpp"

MongoDatabase::MongoDatabase(const std::string &uri, const std::string &dbName)
{
    try
    {
        m_client = mongocxx::client{mongocxx::uri{uri}};
        m_dbName = dbName;
    }
    catch (const mongocxx::exception &e)
    {
        std::cerr << "MongoDB connection error: " << e.what() << std::endl;
        throw;
    }
}

MongoDatabase::~MongoDatabase()
{
}

/**
 * * @brief Insert a map into the MongoDB database
 * * @param bsonBody The BSON document to insert
 * * @return The ID of the inserted document
 */
std::string MongoDatabase::insertMap(const bsoncxx::document::value &bsonBody)
{
    try
    {
        auto collection = m_client[m_dbName]["maps"];
        auto result = collection.insert_one(bsonBody.view());
        if (result)
        {
            std::string inserted_id = result->inserted_id().get_oid().value.to_string();
            return inserted_id;
        }
    }
    catch (const mongocxx::exception &e)
    {
        std::cerr << "MongoDB insert error: " << e.what() << std::endl;
    }
    return "";
}

std::shared_ptr<bsoncxx::document::value> MongoDatabase::getMap(const std::string &mapId)
{
    try
    {
        auto collection = m_client[m_dbName]["maps"];
        bsoncxx::oid oid;
        try
        {
            oid = bsoncxx::oid{mapId};
        }
        catch (const bsoncxx::exception &e)
        {
            std::cerr << "Invalid ObjectId: " << e.what() << std::endl;
            return nullptr;
        }

        auto doc = collection.find_one(bsoncxx::builder::stream::document{} << "_id" << oid << bsoncxx::builder::stream::finalize);
        if (doc)
        {
            return std::make_shared<bsoncxx::document::value>(doc.value());
        }
    }
    catch (const mongocxx::exception &e)
    {
        std::cerr << "MongoDB get error: " << e.what() << std::endl;
    }
    return nullptr;
}