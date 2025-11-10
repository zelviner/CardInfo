#pragma once

#include <zel/myorm.h>

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