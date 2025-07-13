#pragma once

#include "myorm/model.hpp"
#include "myorm/database.h"

class OrderTable : public zel::myorm::Model<OrderTable> {
  public:
    OrderTable()
        : Model() {}
    OrderTable(zel::myorm::Database &db)
        : Model(db()) {}
    OrderTable(zel::myorm::Connection *conn)
        : Model(conn) {}

    void table(const std::string table_name) { table_name_ = table_name; }

    std::string table() const { return table_name_; }

    std::string primary_key() const { return "id"; }

  private:
    std::string table_name_;
};