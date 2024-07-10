#pragma once
#include "ui_main_window.h"
#include <QMainWindow>
#include <vector>
#include <zel/myorm.h>
#include <zel/utility.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();

    void queryBtnClicked();

  private:
    std::vector<std::string> getOrderData(std::string order_id);

    // 初始化窗口
    void initWindow();

    // 初始化UI
    void initUI();

    /// @brief 初始化信号槽
    void initSignalSlot();

    void initConfig(const std::string &inifile);

    bool initConnectionPool();

    bool checkDatabaseConnected();

  private:
    Ui_MainWindow *ui_;

    zel::utility::IniFile      ini_;
    zel::myorm::ConnectionPool pool_;

    std::map<std::string, std::vector<int>> data_configs_;
};
