#include "order.h"
#include "model/data_file.hpp"
#include "model/xh_prjcfg.hpp"

#include <cstring>
#include <zel/myorm.h>
using namespace zel::myorm;

#include <zel/utility.h>
using namespace zel::utility;

#include <string>
#include <vector>

Order::Order(zel::myorm::ConnectionPool *pool, std::map<std::string, std::vector<int>> &data_configs, const std::string &data_files,
             const std::string &card_info)
    : data_configs_(data_configs)
    , data_files_(data_files)
    , card_info_(card_info)
    , pool_(pool) {}

Order::~Order() {}

void Order::run() { getPrdData(data_files_); }

void Order::destroy() { delete this; }

std::map<std::string, std::vector<int>> Order::getDataIndex(const std::string &data_config) {

    std::map<std::string, std::vector<int>> data_indexs;

    auto        data_configs = String::split(data_config, ",");
    std::string header, data = "";
    int         index  = 0;
    int         length = 0;

    // printf("datas: %s\n", datas.c_str());
    for (auto config : data_configs) {

        std::vector<int> indexs;
        header = config.substr(0, config.find(":"));
        length = stoi(config.substr(config.find(":") + 1));
        // data   = datas.substr(index, length);

        indexs.push_back(index);
        indexs.push_back(length);

        index += length;

        int interception = header.find("PRINT");
        if (interception != std::string::npos) {
            data_indexs[header.substr(interception + 6)] = indexs;
        }
    }

    return data_indexs;
}

void Order::getPrdData(const std::string &prd_file) {

    // printf("finding %s.\n", prd_file.c_str());

    auto start = std::chrono::system_clock::now();

    std::string sql     = "select DataContent from " + prd_file;
    auto        conn    = pool_->get();
    auto        prd_db  = Database(conn);
    auto        records = prd_db.query(sql);
    pool_->put(conn);

    auto center = std::chrono::system_clock::now();
    // printf("data size: %d\n", all.size());
    for (auto record : records) {
        for (auto it = record.begin(); it != record.end(); it++) {
            std::string data = it->second;
            if (getDataInfo(data)) {
                printf("find card info in %s.\n", prd_file.c_str());
            }
        }
    }

    auto end = std::chrono::system_clock::now();

    auto time_one = std::chrono::duration_cast<std::chrono::milliseconds>(center - start).count();
    auto time_two = std::chrono::duration_cast<std::chrono::milliseconds>(end - center).count();
    // printf("%s done, use time_one: %lld ms, time_two: %lld ms\n", prd_file.c_str(), time_one, time_two);
}

bool Order::getDataInfo(const std::string &datas) {

    for (auto config : data_configs_) {
        auto header = config.first;
        auto data   = datas.substr(config.second[0], config.second[1]);

        if (data == card_info_) {
            return true;
        }

        // printf("%s: %s\n", header.c_str(), data.c_str());
    }

    return false;
}
