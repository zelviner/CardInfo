#include "order.h"
#include "model/order_table.hpp"
#include "model/xh_prjcfg.hpp"

#include <cstring>
#include <memory>
#include <zel/myorm.h>
using namespace zel::myorm;

#include <zel/utility.h>
using namespace zel::utility;

#include <string>
#include <vector>

Order::Order(std::shared_ptr<ConnectionPool> remote_pool, std::shared_ptr<ConnectionPool> local_pool, std::map<std::string, std::vector<int>> &data_configs,
             std::vector<std::string> data_files, const std::string &card_info, const std::string &order_id)
    : data_configs_(data_configs)
    , data_files_(data_files)
    , card_info_(card_info)
    , local_pool_(local_pool)
    , remote_pool_(remote_pool)
    , order_id_(order_id) {}

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

void Order::getPrdData(std::vector<std::string> prd_files) {

    auto start = std::chrono::system_clock::now();

    std::string sql = "";
    for (int i = 0; i < prd_files.size(); i++) {
        sql += "select DataContent from `" + prd_files[i] + "`";
        if (i != prd_files.size() - 1) {
            sql += "\nunion all\n";
        }
    }

    auto conn    = remote_pool_->get();
    auto prd_db  = Database(conn);
    auto records = prd_db.query(sql);
    remote_pool_->put(conn);

    std::vector<OrderTable> rows;

    int  index  = 0;
    int  i      = 0;
    auto center = std::chrono::system_clock::now();
    for (auto record : records) {
        for (auto it = record.begin(); it != record.end(); it++) {
            i++;
            std::string datas       = it->second;
            auto        order_table = OrderTable();
            order_table["datafile"] = prd_files[0];
            for (auto config : data_configs_) {
                auto header         = config.first;
                auto data           = datas.substr(config.second[0], config.second[1]);
                order_table[header] = data;
            }
            rows.push_back(order_table);
        }
    }

    if (index != 0) {
        index = index / (records.size() / prd_files.size());
        printf("find card info in %s.\n", prd_files[index].c_str());
    }

    auto local_conn  = local_pool_->get();
    auto order_table = OrderTable(local_conn);
    order_table.table(order_id_);
    order_table.insert(rows);
    local_pool_->put(local_conn);

    auto end = std::chrono::system_clock::now();

    auto time_one = std::chrono::duration_cast<std::chrono::milliseconds>(center - start).count();
    auto time_two = std::chrono::duration_cast<std::chrono::milliseconds>(end - center).count();
    printf("thead done, use time_one: %lld ms, time_two: %lld ms\n", time_one, time_two);
}
