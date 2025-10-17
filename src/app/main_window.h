#pragma once
#include "download_loading.h"
#include "task/download.h"
#include "ui_main_window.h"

#include <QMainWindow>
#include <memory>
#include <vector>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();

    /// @brief 查询按钮点击事件
    void queryBtnClicked();

    /// @brief 删除订单按钮点击事件
    void deleteBtnClicked();

    /// @brief 保存设置按钮点击事件
    void saveBtnClicked();

  public slots:
    void failure();

    void success();

    void notFound();

    void showResult(const QString &filename, const QString &iccid, const QString &puk);

  private:
    /// @brief 初始化窗口
    void init_window();

    /// @brief 初始化信号槽
    void init_signals_slots();

    /// @brief 初始化日志
    void init_logger();

    /// @brief 初始化配置文件
    /// @param inifile 配置文件名
    void init_config(const std::string &inifile);

    /// @brief 初始化数据库连接池
    bool init_connection_pool();

    /// @brief 检查数据库是否可以连接
    /// @param mysql_ini 数据库配置
    bool check_database_connected(std::map<std::string, Value> mysql_ini);

    /// @brief 初始化界面
    void init_ui();

    /// @brief  订单数据是否存在
    bool order_data_is_exist();

    /// @brief Get the Data Files object
    /// @return std::vector<std::string>  订单文件列表
    std::vector<std::string> data_files();

    /// @brief Get the Data Index object
    std::map<std::string, std::vector<int>> data_config(const std::string &data_config_str);

    /// @brief  查询卡信息
    bool query();

  private:
    Ui_MainWindow   *ui_;
    DownloadLoading *download_loading_;
    Download        *download_;

    std::shared_ptr<Data> data_;
};
