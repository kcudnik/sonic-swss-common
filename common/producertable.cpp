#include "common/redisreply.h"
#include "common/producertable.h"
#include "common/json.h"
#include "common/json.hpp"
#include "common/logger.h"
#include <stdlib.h>
#include <tuple>

using namespace std;
using json = nlohmann::json;

namespace swss {

ProducerTable::ProducerTable(DBConnector *db, string tableName) :
    Table(db, tableName)
{
}

ProducerTable::ProducerTable(DBConnector *db, string tableName, string dumpFile) :
    Table(db, tableName)
{
    m_dumpFile.open(dumpFile, fstream::out | fstream::trunc);
    m_dumpFile << "[" << endl;
}

ProducerTable::~ProducerTable() {
    if (m_dumpFile.is_open())
    {
        m_dumpFile << endl << "]" << endl;
        m_dumpFile.close();
    }
}

void ProducerTable::enqueueDbChange(string key, string value, string op)
{
    static std::string luaScript =
        "redis.call('LPUSH', KEYS[1], ARGV[1]);"
        "redis.call('LPUSH', KEYS[2], ARGV[2]);"
        "redis.call('LPUSH', KEYS[3], ARGV[3]);"
        "redis.call('PUBLISH', KEYS[4], ARGV[4]);";

    static std::string sha = scriptLoad(luaScript);

    char *temp;

    int len = redisFormatCommand(
            &temp,
            "EVALSHA %s 4 %s %s %s %s %s %s %s %s",
            sha.c_str(),
            getKeyQueueTableName().c_str(),
            getValueQueueTableName().c_str(),
            getOpQueueTableName().c_str(),
            getChannelTableName().c_str(),
            key.c_str(),
            value.c_str(),
            op.c_str(),
            "G");

    if (len < 0)
    {
        SWSS_LOG_ERROR("redisFormatCommand failed");
        throw std::runtime_error("fedisFormatCommand failed");
    }

    string command = string(temp, len);
    free(temp);

    RedisReply r(m_db, command, REDIS_REPLY_NIL, true);
}

void ProducerTable::set(string key, vector<FieldValueTuple> &values, string op)
{
    if (m_dumpFile.is_open())
    {
        if (!m_firstItem)
            m_dumpFile << "," << endl;
        else
            m_firstItem = false;

        json j;
        string json_key = getKeyName(key);
        j[json_key] = json::object();
        for (auto it : values)
            j[json_key][fvField(it)] = fvValue(it);
        j["OP"] = op;
        m_dumpFile << j.dump(4);
    }

    enqueueDbChange(key, JSon::buildJson(values), "S" + op);
}

void ProducerTable::del(string key, string op)
{
    if (m_dumpFile.is_open())
    {
        if (!m_firstItem)
            m_dumpFile << "," << endl;
        else
            m_firstItem = false;

        json j;
        string json_key = getKeyName(key);
        j[json_key] = json::object();
        j["OP"] = op;
        m_dumpFile << j.dump(4);
    }

    enqueueDbChange(key, "{}", "D" + op);
}

}
