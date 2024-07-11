#include "download_loading.h"

DownloadLoading::DownloadLoading(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(std::make_unique<Ui_DownloadLoading>()) {
    ui_->setupUi(this);

    initWindow();

    initUI();

    initSignalSlot();
}

DownloadLoading::~DownloadLoading() {}

void DownloadLoading::initWindow() { setWindowTitle("索引中 ..."); }

void DownloadLoading::initUI() {}

void DownloadLoading::initSignalSlot() {}
