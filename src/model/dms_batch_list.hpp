#pragma once

#include <zel/myorm.h>

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