#include "order.h"

#include "model/dms_product_orders.hpp"

Order::Order(const std::shared_ptr<zel::myorm::Database> &db)
    : db_(db) {}

Order::~Order() {}

std::vector<std::string> Order::orders() {
    std::vector<std::string> orders;

    DmsProductOrders dms_product_orders(*db_);
    auto             all = dms_product_orders.order("desc").all();
    for (auto &one : all) {
        std::string order_no = one("Code").asString();
        orders.push_back(order_no);
    }

    return orders;
}

bool Order::exists(const std::string &order_no) {
    DmsProductOrders dms_product_orders(*db_);
    dms_product_orders.where("Code", order_no);
    auto result = dms_product_orders.all();
    return result.size() > 0;
}

CardInfo Order::cardInfo(const std::string &order_no, const std::string &card_no) {
    DmsProductOrders dms_product_orders(*db_);
    dms_product_orders.where("Code", order_no);
    auto record   = dms_product_orders.one();
    int  order_id = record("ID").asInt();
}