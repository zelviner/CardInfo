#include "download_loading.h"
#include <qnamespace.h>

DownloadLoading::DownloadLoading(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(std::make_unique<Ui_DownloadLoading>()) {
    ui_->setupUi(this);

    init_window();

    init_ui();

    init_signals_slots();
}

DownloadLoading::~DownloadLoading() {}

void DownloadLoading::threadDown(int down) { ui_->progressBar->setValue(down); }

void DownloadLoading::init_window() {
    setWindowTitle("索引中 ...");
    setWindowModality(Qt::ApplicationModal);
}

void DownloadLoading::init_ui() {}

void DownloadLoading::init_signals_slots() {}
