#pragma once

#include "myorm/database.h"
#include "myorm/model.hpp"

class DmsPersoData : public zel::myorm::Model<DmsPersoData> {
  public:
    DmsPersoData()
        : Model() {}
    DmsPersoData(zel::myorm::Database &db, const std::string &table_name, const std::string &primary_key_name)
        : Model(db())
        , table_name_(table_name)
        , primary_key_name_(primary_key_name) {}
    DmsPersoData(zel::myorm::Connection *conn, const std::string &table_name, const std::string &primary_key_name)
        : Model(conn)
        , table_name_(table_name)
        , primary_key_name_(primary_key_name) {}

    std::string table() const { return table_name_; }

    std::string primary_key() const { return primary_key_name_; }

  private:
    std::string table_name_;
    std::string primary_key_name_;
};