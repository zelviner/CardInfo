#include "order.h"
#include "model/order_table.hpp"

#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <zel.h>

using namespace zel::myorm;
using namespace zel::utility;

Order::Order() {}

Order::Order(Data *data)
    : zel::thread::Task((void *) data) {}

Order::~Order() {}

void Order::run() {

    auto data  = (Data *) data_;
    auto start = std::chrono::system_clock::now();

    std::string sql     = "select DataContent from `" + data->file + "`";
    auto        conn    = data->data_pool->get();
    auto        prd_db  = Database(conn);
    auto        records = prd_db.query(sql);
    data->data_pool->put(conn);

    std::vector<OrderTable> rows;

    auto center = std::chrono::system_clock::now();

    for (auto record : records) {
        for (auto it = record.begin(); it != record.end(); it++) {
            std::string datas       = it->second;
            auto        order_table = OrderTable();
            order_table["datafile"] = data->file;
            for (auto config : data->configs) {
                auto header         = config.first;
                auto data           = datas.substr(config.second[0], config.second[1]);
                order_table[header] = data;
            }
            rows.push_back(order_table);
        }
    }

    auto local_conn  = data->print_pool->get();
    auto order_table = OrderTable(local_conn);
    order_table.table(data->order_id);
    order_table.insert(rows);
    data->print_pool->put(local_conn);

    auto end = std::chrono::system_clock::now();

    auto time_one = std::chrono::duration_cast<std::chrono::milliseconds>(center - start).count();
    auto time_two = std::chrono::duration_cast<std::chrono::milliseconds>(end - center).count();
    printf("thead %ld done, use time_one: %lld ms, time_two: %lld ms\n", std::this_thread::get_id(), time_one, time_two);
}

void Order::destroy() {
    auto data = (Data *) data_;
    if (data != nullptr) {
        delete data;
        data_ = nullptr;
    }

    delete this;
}