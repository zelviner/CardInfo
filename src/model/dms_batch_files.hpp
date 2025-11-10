#pragma once

#include <zel/myorm.h>

class DmsBatchFiles : public zel::myorm::Model<DmsBatchFiles> {
  public:
    DmsBatchFiles()
        : Model() {}
    DmsBatchFiles(zel::myorm::Database &db)
        : Model(db()) {}
    DmsBatchFiles(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "dms_batchfiles"; }

    std::string primary_key() const { return "ID"; }
};