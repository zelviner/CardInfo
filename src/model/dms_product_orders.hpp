#pragma once

#include "myorm/database.h"
#include "myorm/model.hpp"

class DmsProductOrders : public zel::myorm::Model<DmsProductOrders> {
  public:
    DmsProductOrders()
        : Model() {}
    DmsProductOrders(zel::myorm::Database &db)
        : Model(db()) {}
    DmsProductOrders(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "dms_productorders"; }

    std::string primary_key() const { return "ID"; }
};