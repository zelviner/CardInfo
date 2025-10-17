#pragma once

#include "order/data.hpp"

#include <memory>
#include <qthread>

class Download : public QThread {
    Q_OBJECT
  public:
    Download(std::shared_ptr<Data> data);
    ~Download();

    // 重写run函数，在这里执行线程的工作
    void run() override;

  signals:
    // 信号函数，用于向外界发射信号
    void failure();
    void success();
    void threadDown(int down);

  private:
    bool create_order_table();

    bool download();

  private:
    std::shared_ptr<Data> data_;
};