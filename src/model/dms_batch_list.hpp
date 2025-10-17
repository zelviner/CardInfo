#pragma once

#include "myorm/database.h"
#include "myorm/model.hpp"

class DmsBatchList : public zel::myorm::Model<DmsBatchList> {
  public:
    DmsBatchList()
        : Model() {}
    DmsBatchList(zel::myorm::Database &db)
        : Model(db()) {}
    DmsBatchList(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "dms_batchlist"; }

    std::string primary_key() const { return "ID"; }
};