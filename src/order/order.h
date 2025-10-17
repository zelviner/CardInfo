#pragma once

#include "card_info.h"
#include "myorm/database.h"


#include <memory>
#include <vector>

class Order {

  public:
    Order(const std::shared_ptr<zel::myorm::Database> &db);
    ~Order();

    /// @brief Get all orders
    std::vector<std::string> orders();

    /// @brief
    bool exists(const std::string &order_no);

    CardInfo cardInfo(const std::string &order_no, const std::string &card_no);

  private:
    std::shared_ptr<zel::myorm::Database> db_;
};