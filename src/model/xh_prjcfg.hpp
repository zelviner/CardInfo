#pragma once

#include <zel/myorm.h>

class XhPrjcfg : public zel::myorm::Model<XhPrjcfg> {
  public:
    XhPrjcfg()
        : Model() {}
    XhPrjcfg(zel::myorm::Database &db)
        : Model(db()) {}
    XhPrjcfg(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "xh_prjcfg"; }

    std::string primary_key() const { return "ID"; }
};