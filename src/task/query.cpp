#include "query.h"
#include "model/order_table.hpp"

#include <memory>

Query::Query(std::shared_ptr<Data> data)
    : data_(data) {}

Query::~Query() {}

void Query::run() {

    std::string field = "";
    for (auto config : data_->configs) {
        if (data_->card_info.size() == config.second[1]) field = config.first;
    }

    if (field == "") field = "ICCID";

    auto conn        = data_->print_pool->get();
    auto order_table = OrderTable(conn);
    order_table.table(data_->order_id);
    auto records = order_table.where(field, "=", data_->card_info).all();
    data_->print_pool->put(conn);

    if (records.size() != 1) {
        emit notFound();
    }

    for (auto record : records) {
        std::string filename = record("datafile");
        std::string iccid    = record("ICCID");
        std::string puk      = record("PUK1");

        emit showResult(QString(filename.c_str()), QString(iccid.c_str()), QString(puk.c_str()));
    }
}