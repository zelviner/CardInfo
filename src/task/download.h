#pragma once

#include <memory>
#include <zel/myorm.h>
#include <zel/utility.h>

#include <qthread>

class Download : public QThread {
    Q_OBJECT
  public:
    Download(std::shared_ptr<zel::utility::IniFile> ini, std::shared_ptr<zel::myorm::ConnectionPool> remote_pool,
             std::shared_ptr<zel::myorm::ConnectionPool> local_pool, const std::string &order_id, const std::string &card_info,
             std::vector<std::vector<std::string>> &data_files, std::map<std::string, std::vector<int>> &data_configs);
    ~Download();

    // 重写run函数，在这里执行线程的工作
    void run() override;

  signals:
    // 信号函数，用于向外界发射信号
    void failure();
    void success();
    void threadDown(int down);

  private:
    std::vector<std::vector<std::string>> getOrderData();

    bool createOrderTable();

    bool download();

  private:
    std::shared_ptr<zel::utility::IniFile>      ini_;
    std::shared_ptr<zel::myorm::ConnectionPool> remote_pool_;
    std::shared_ptr<zel::myorm::ConnectionPool> local_pool_;

    std::string                             card_info_;
    std::string                             order_id_;
    std::map<std::string, std::vector<int>> data_configs_;
    std::vector<std::vector<std::string>>   data_files_;
};