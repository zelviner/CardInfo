#include "order.h"
#include "model/data_file.hpp"
#include "model/xh_prjcfg.hpp"

#include <string>
#include <vector>
#include <zel/myorm.h>
using namespace zel::myorm;

Order::Order(zel::myorm::ConnectionPool *pool, const std::string &order_id, const std::string &card_info)
    : order_id_(order_id)
    , card_info_(card_info)
    , pool_(pool) {}

Order::~Order() {}

void Order::query() {

    getOrderData();

    for (auto data_file : data_files_) {
        // printf("%s\n", data_file.c_str());
        getPrdData(data_file);
    }
}

void Order::getOrderData() {
    auto conn    = pool_->get();
    auto all     = XhPrjcfg(conn).where("PrjId", "=", order_id_).all();
    data_config_ = all[0]("DataCfgA").asString();

    for (auto one : all) {
        std::string data_file = one("DataFiles");

        if (data_file.find(".prd") != std::string::npos) {
            data_files_.push_back(data_file);
        }
    }

    pool_->put(conn);
}

void Order::getPrdData(const std::string &prd_file) {
    auto conn = pool_->get();

    auto model = DataFile(conn);
    model.table(prd_file);

    auto all = model.all();

    for (auto one : all) {
        printf("%s\n", one.str().c_str());
    }

    pool_->put(conn);
    // std::string sql = "SELECT 'DataContent' FROM " + prd_file;
}