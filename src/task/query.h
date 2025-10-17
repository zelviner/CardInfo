#pragma once

#include "myorm/connection_pool.h"
#include "order/card_info.h"

#include <memory>
#include <qthread>

class Query : public QThread {
    Q_OBJECT
  public:
    Query(std::shared_ptr<zel::myorm::ConnectionPool> connection_pool, const std::string &order_no, const std::string &card_no, int start_id, int end_id);

    ~Query();

    void run() override;

  signals:
    void notFound();
    void found(std::shared_ptr<CardInfo> card_info);

  private:
    std::shared_ptr<zel::myorm::ConnectionPool> connection_pool_;
    std::string                                 order_no_;
    std::string                                 card_no_;
    int                                         start_id_;
    int                                         end_id_;
};