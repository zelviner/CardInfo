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

Order::Order(zel::myorm::ConnectionPool *pool, const std::string &data_config, const std::string &data_files, const std::string &card_info)
    : data_config_(data_config)
    , data_files_(data_files)
    , card_info_(card_info)
    , pool_(pool) {}

Order::~Order() {}

void Order::run() { getPrdData(data_files_); }

void Order::destroy() { delete this; }

void Order::query() {

    // getOrderData();

    // for (auto data_file : data_files_) {
    //     // printf("%s\n", data_file.c_str());
    //     getPrdData(data_file);
    // }
}

// void Order::getOrderData() {
//     auto conn             = pool_->get();
//     auto all              = XhPrjcfg(conn).where("PrjId", "=", order_id_).all();
//     data_config_          = all[0]("DataCfgA").asString();
//     std::string data_file = "";

//     for (int i = 0; i < all.size(); i++) {
//         std::string str = all[i]("DataFiles");

//         if (str.find(".prd") == std::string::npos) {
//             continue;
//         }

//         String::toLower(str);

//         if ((i + 1) % 5 == 0) {
//             data_file += "`" + str + "`;";
//             data_files_.push_back(data_file.substr(0, data_file.size() - 1));
//             data_file = "";
//         } else {
//             data_file += "`" + str + "`;";
//         }
//     }

//     if (data_file != "") {
//         data_files_.push_back(data_file.substr(0, data_file.size() - 1));
//         data_file = "";
//     }

//     pool_->put(conn);
// }

void Order::getPrdData(const std::string &prd_file) {

    printf("finding %s.\n", prd_file.c_str());

    auto start = std::chrono::system_clock::now();

    std::string sql     = "select DataContent from " + prd_file;
    auto        conn    = pool_->get();
    auto        prd_db  = Database(conn);
    auto        records = prd_db.query(sql);
    auto center = std::chrono::system_clock::now();
    pool_->put(conn);


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
    printf("%s done, use time_one: %lld ms, time_two: %lld ms\n", prd_file.c_str(), time_one, time_two);
}

bool Order::getDataInfo(const std::string &datas) {

    auto        data_configs = String::split(data_config_, ",");
    std::string header, data = "";
    int         index  = 0;
    int         length = 0;

    // printf("datas: %s\n", datas.c_str());
    for (auto config : data_configs) {
        header = config.substr(0, config.find(":"));
        length = stoi(config.substr(config.find(":") + 1));
        data   = datas.substr(index, length);
        index += length;
        if (header.find("PRINT") != std::string::npos) {
            // printf("header: %s length: %d data: %s\n", header.c_str(), length, data.c_str());
            if (card_info_ == data) {
                return true;
            }
        }
    }

    return false;
}