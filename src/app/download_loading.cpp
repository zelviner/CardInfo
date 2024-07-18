#include "download_loading.h"
#include <qnamespace.h>

DownloadLoading::DownloadLoading(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(std::make_unique<Ui_DownloadLoading>()) {
    ui_->setupUi(this);

    initWindow();

    initUI();

    initSignalSlot();
}

DownloadLoading::~DownloadLoading() {}

void DownloadLoading::threadDown(int down) { ui_->progressBar->setValue(down); }

void DownloadLoading::initWindow() {
    setWindowTitle("索引中 ...");
    setWindowModality(Qt::ApplicationModal);
}

void DownloadLoading::initUI() {}

void DownloadLoading::initSignalSlot() {}
