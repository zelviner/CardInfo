#pragma once
#include "ui_main_window.h"
#include <QMainWindow>
#include <zel/myorm.h>
#include <zel/utility.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QMainWindow *parent = nullptr);
    ~MainWindow();

    void queryBtnClicked();

  private:
    // 初始化窗口
    void initWindow();

    // 初始化UI
    void initUI();

    /// @brief 初始化信号槽
    void initSignalSlot();

    void initConfig(const std::string &inifile);

    void initConnectionPool();

  private:
    Ui_MainWindow *ui_;

    zel::utility::IniFile      ini_;
    zel::myorm::ConnectionPool pool_;
};
