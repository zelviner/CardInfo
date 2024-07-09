#pragma once

#include <zel/myorm.h>

class DataFile : public zel::myorm::Model<DataFile> {
  public:
    DataFile()
        : Model() {}
    DataFile(zel::myorm::Database &db)
        : Model(db()) {}
    DataFile(zel::myorm::Connection *conn)
        : Model(conn) {}

    void table(const std::string table_name) { table_name_ = table_name; }

    std::string table() const { return table_name_; }

    std::string primary_key() const { return "ID"; }

  private:
    std::string table_name_;
};