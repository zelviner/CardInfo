#include "order.h"

#include "card_info.h"
#include "model/dms_batch_files.hpp"
#include "model/dms_batch_list.hpp"
#include "model/dms_order_conf.hpp"
#include "model/dms_perso_data.hpp"
#include "model/dms_product_orders.hpp"

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <zel/utility/string.h>

using namespace zel::utility;

Order::Order(zel::myorm::Connection *connection)
    : card_info_(nullptr)
    , connection_(connection) {}

Order::~Order() {}

std::vector<std::string> Order::orders() {
    std::vector<std::string> orders;

    DmsProductOrders dms_product_orders(connection_);
    auto             all = dms_product_orders.all();
    // 反转
    std::reverse(all.begin(), all.end());
    for (auto &one : all) {
        std::string order_no = one("Code").asString();
        orders.push_back(order_no);
    }

    return orders;
}

bool Order::exists(const std::string &order_no) {
    DmsProductOrders dms_product_orders(connection_);
    dms_product_orders.where("Code", order_no);
    auto result = dms_product_orders.all();
    return result.size() > 0;
}

int Order::dataSize(const std::string &order_no) {
    perso_data_table(order_no);
    DmsPersoData dms_perso_data(connection_, perso_data_table_, "ID");
    return dms_perso_data.count("ID");
}

std::shared_ptr<CardInfo> Order::query(const std::string &order_no, const std::string &card_no, int start_id, int end_id) {
    perso_data_table(order_no);
    auto perso_datas = perso_data(order_no, start_id, end_id);

    for (auto &card_info : perso_datas) {
        if (card_info->print_data.find(card_no) != std::string::npos) {
            card_info_            = card_info;
            auto file_record      = DmsBatchFiles(connection_).where("ID", card_info_->file_id).one();
            card_info_->file_name = file_record("Filename").asString();
            if (card_info_->print_data.size() > 40) {
                card_info_->serial_number = card_info_->print_data.substr(22, 16);
                query_barcode(order_no, card_info_->serial_number);
            } else {
                card_info_->serial_number = "";
            }
            return card_info_;
        }
    }

    return nullptr;
}

void Order::perso_data_table(const std::string &order_no) {
    // 查询订单ID
    auto order_record = DmsProductOrders(connection_).where("Code", order_no).one();
    int  order_id     = order_record("ID").asInt();

    // 查询批次ID
    auto batch_record = DmsOrderConf(connection_).where("Order", order_id).one();
    int  batch_id     = batch_record("Batch").asInt();

    // 查询个人化数据表
    auto        perso_data_record = DmsBatchList(connection_).where("ID", batch_id).one();
    std::string perso_data_table  = perso_data_record("Uuid").asString();
    String::toLower(perso_data_table);
    perso_data_table_ = perso_data_table;
}

std::vector<std::shared_ptr<CardInfo>> Order::perso_data(const std::string &order_no, int start_id, int end_id) {
    // 查询个人化数据
    std::vector<std::shared_ptr<CardInfo>> perso_datas;

    std::stringstream oss;
    oss << "SELECT Print, Iccid, Imsi, File FROM `" << perso_data_table_ << "` WHERE ID >= " << start_id << " AND ID < " << end_id;
    zel::myorm::Database db(connection_);
    auto                 perso_data_records = db.query(oss.str());
    for (auto &one : perso_data_records) {
        std::shared_ptr<CardInfo> card_info = std::make_shared<CardInfo>();
        card_info->file_id                  = one["File"].asInt();
        card_info->print_data               = one["Print"].asString();

        auto iccid = one["Iccid"].asString();
        exchange_iccid(iccid);
        card_info->iccid = iccid;

        auto imsi = one["Imsi"].asString();
        exchange_iccid(imsi);
        card_info->imsi = imsi;

        perso_datas.push_back(card_info);
    }

    return perso_datas;
}

void Order::query_barcode(const std::string &order_no, const std::string &iccid) {
    std::stringstream oss;

    // 查询盒号
    oss << "SELECT box_number FROM `box_data`.`" << order_no << "` WHERE '" << iccid << "' BETWEEN start_number AND end_number";
    zel::myorm::Database box_data_db(connection_);
    auto                 box_data_records = box_data_db.query(oss.str());
    card_info_->box_number                = box_data_records[0]["box_number"].asString();

    // 清空字符串流
    oss.str("");
    oss.clear();

    // 查询盒号
    oss << "SELECT carton_number FROM `carton_data`.`" << order_no << "` WHERE '" << iccid << "' BETWEEN start_number AND end_number";
    zel::myorm::Database carton_data_db(connection_);
    auto                 carton_data_records = carton_data_db.query(oss.str());
    card_info_->carton_number                = carton_data_records[0]["carton_number"].asString();
}

void Order::exchange_iccid(std::string &iccid) {
    for (size_t i = 0; i + 1 < iccid.size(); i += 2) {
        std::swap(iccid[i], iccid[i + 1]);
    }
}