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

    /// @brief 查询按钮点击事件
    void queryBtnClicked();

    /// @brief 删除订单按钮点击事件
    void deleteBtnClicked();

  public slots:
    void failure();
    void success();

  private:
    /// @brief 初始化窗口
    void initWindow();

    /// @brief 初始化信号槽
    void initSignalSlot();

    /// @brief 初始化日志
    void initLogger();

    /// @brief 初始化配置文件
    /// @param inifile 配置文件名
    void initConfig(const std::string &inifile);

    /// @brief 初始化数据库连接池
    bool initConnectionPool();

    /// @brief 检查数据库是否可以连接
    /// @param mysql_ini 数据库配置
    bool checkDatabaseConnected(std::map<std::string, Value> mysql_ini);

    /// @brief 初始化界面
    void initUI();

    bool download();

    std::vector<std::vector<std::string>> getOrderData();

    bool query();

  private:
    Ui_MainWindow   *ui_;
    DownloadLoading *download_loading_;

    std::shared_ptr<zel::utility::IniFile>      ini_;
    std::shared_ptr<zel::myorm::ConnectionPool> data_pool_;
    std::shared_ptr<zel::myorm::ConnectionPool> print_pool_;

    std::string                             card_info_;
    std::string                             order_id_;
    std::map<std::string, std::vector<int>> data_configs_;
    std::vector<std::vector<std::string>>   data_files_;
};
