#pragma once

#include <zel/myorm.h>

class XhTaskList : public zel::myorm::Model<XhTaskList> {
  public:
    XhTaskList()
        : Model() {}
    XhTaskList(zel::myorm::Database &db)
        : Model(db()) {}
    XhTaskList(zel::myorm::Connection *conn)
        : Model(conn) {}

    std::string table() const { return "xh_task_list"; }

    std::string primary_key() const { return "ID"; }
};