#pragma once

#include "card_info.h"

#include <memory>
#include <vector>
#include <zel/myorm.h>

class Order {

  public:
    Order(zel::myorm::Connection *connection);
    ~Order();

    /// @brief Get all orders
    std::vector<std::string> orders();

    /// @brief
    bool exists(const std::string &order_no);

    int dataSize(const std::string &order_no);

    std::shared_ptr<CardInfo> query(const std::string &order_no, const std::string &card_no, int start_id, int end_id);

  private:
    void perso_data_table(const std::string &order_no);

    std::vector<std::shared_ptr<CardInfo>> perso_data(int start_id, int end_id);

    void query_barcode(const std::string &order_no, const std::string &iccid);

    void exchange_iccid(std::string &iccid);

  private:
    std::shared_ptr<CardInfo> card_info_;
    zel::myorm::Connection   *connection_;
    std::string               perso_data_table_;
};