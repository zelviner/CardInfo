#pragma once

#include "ui_download_loading.h"

#include <memory>
#include <qmainwindow>
#include <qobjectdefs.h>

class DownloadLoading : public QMainWindow {
    Q_OBJECT
  public:
    DownloadLoading(QMainWindow *parent = nullptr);
    ~DownloadLoading();

  public slots:
    void threadDown(int down);

  private:
    // 初始化窗口
    void initWindow();

    // 初始化UI
    void initUI();

    /// @brief 初始化信号槽
    void initSignalSlot();

  private:
    std::unique_ptr<Ui_DownloadLoading> ui_;
};