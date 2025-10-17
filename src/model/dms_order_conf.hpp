#pragma once

#include "myorm/database.h"
#include "myorm/model.hpp"


class DmsOrderConf : public zel::myorm::Model<DmsOrderConf> {
  public:
    DmsOrderConf()
        : Model() {}
    DmsOrderConf(zel::myorm::Database &db)
        : Model(db()) {}
    DmsOrderConf(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "dms_orderconf"; }

    std::string primary_key() const { return "ID"; }
};