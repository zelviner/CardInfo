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
    void init_window();

    // 初始化UI
    void init_ui();

    /// @brief 初始化信号槽
    void init_signals_slots();

  private:
    std::unique_ptr<Ui_DownloadLoading> ui_;
};