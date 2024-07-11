#pragma once
#include "download_loading.h"
#include "task/download.h"
#include "ui_main_window.h"

#include <QMainWindow>
#include <memory>
#include <vector>
#include <zel/myorm.h>
#include <zel/utility.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();

    void queryBtnClicked();

  public slots:
    void failure();
    void success();

  private:
    // 初始化窗口
    void initWindow();

    /// @brief 初始化信号槽
    void initSignalSlot();

    void initLogger();

    void initConfig(const std::string &inifile);

    bool initConnectionPool();

    bool checkDatabaseConnected(std::map<std::string, Value> mysql_ini);

    // 初始化UI
    void initUI();

    bool download();

    std::vector<std::vector<std::string>> getOrderData();

    bool createOrderTable();

    bool downloadData();

    bool query();

  private:
    Ui_MainWindow   *ui_;
    DownloadLoading *download_loading_;

    std::shared_ptr<zel::utility::IniFile>      ini_;
    std::shared_ptr<zel::myorm::ConnectionPool> remote_pool_;
    std::shared_ptr<zel::myorm::ConnectionPool> local_pool_;

    std::string                             card_info_;
    std::string                             order_id_;
    std::map<std::string, std::vector<int>> data_configs_;
    std::vector<std::vector<std::string>>   data_files_;
};
