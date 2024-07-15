#include "main_window.h"
#include "download_loading.h"
#include "model/order_table.hpp"
#include "model/xh_prjcfg.hpp"
#include "task/download.h"
#include "task/order.h"

#include <qpushbutton.h>
#include <zel/thread.h>
using namespace zel::thread;

#include <zel/utility.h>
using namespace zel::utility;
using namespace zel::myorm;

#include <chrono>
#include <cstdlib>
#include <memory>
#include <qmessagebox.h>
#include <qmessagebox>
#include <type_traits>
#include <vector>

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow) {
    ui_->setupUi(this);

    initWindow();

    initSignalSlot();

    initLogger();

    initConfig("config.ini");

    initConnectionPool();

    initUI();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::queryBtnClicked() {
    order_id_  = ui_->order_id_box->currentText().toStdString();
    card_info_ = ui_->card_info_line->text().toStdString();
    ui_->result_group_box->setVisible(false);
    ui_->not_found_label->setVisible(false);

    if (order_id_ == "" || card_info_ == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
        return;
    }

    data_files_ = getOrderData();

    if (data_files_.size() == 0) {
        QMessageBox::critical(this, "警告", "未找到该订单，请检查订单号是否正确");
        return;
    }

    // 查询订单表是否存在
    if (download()) {

        // 弹出下载加载窗口, 并停留
        download_loading_ = new DownloadLoading(this);
        download_loading_->show();

        // 创建工作线程
        auto download = new Download(ini_, data_pool_, print_pool_, order_id_, card_info_, data_files_, data_configs_);

        // 连接信号槽
        connect(download, &Download::success, this, &MainWindow::success);
        connect(download, &Download::threadDown, download_loading_, &DownloadLoading::threadDown);

        // 启动工作线程
        download->start();
    } else {
        // 查询
        query();
    }
}

void MainWindow::deleteBtnClicked() {

    auto order_id = ui_->order_id_box->currentText();

    auto ques = QMessageBox::question(this, "提示", "确定删除订单 " + order_id + " 吗？", QMessageBox::Yes | QMessageBox::No);

    if (ques == QMessageBox::No) return;

    auto        conn = print_pool_->get();
    Database    db(conn);
    std::string sql = "drop table if exists `" + order_id.toStdString() + "`";
    db.execute(sql);
    print_pool_->put(conn);
}

void MainWindow::success() {

    if (download_loading_ != nullptr) {
        delete download_loading_;
    }

    query();
}

void MainWindow::failure() {}

std::vector<std::vector<std::string>> MainWindow::getOrderData() {
    std::vector<std::vector<std::string>> data_files;

    auto conn = data_pool_->get();
    auto all  = XhPrjcfg(conn).where("PrjId", "=", order_id_).all();
    data_pool_->put(conn);

    if (all.size() == 0) {
        return data_files;
    }

    std::string data_config = all[0]("DataCfgA").asString();

    data_configs_ = Order::getDataIndex(data_config);

    int table_count = (*ini_)["system"]["table_count"];

    std::vector<std::string> temp_files;
    for (int i = 0; i < all.size(); i++) {
        std::string data_file = all[i]("DataFiles");

        if (data_file.find(".prd") == std::string::npos) {
            continue;
        }

        String::toLower(data_file);

        temp_files.push_back(data_file);
        if ((i + 1) % table_count == 0) {
            data_files.push_back(temp_files);
            temp_files.clear();
        }
    }

    return data_files;
}

void MainWindow::initWindow() {
    // 设置窗口标题
    setWindowTitle("星汉卡片信息");
}

void MainWindow::initSignalSlot() {
    connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked);
    connect(ui_->delete_btn, &QPushButton::clicked, this, &MainWindow::deleteBtnClicked);
}

void MainWindow::initLogger() {
    auto logger = Logger::instance();
    logger->open("error.log");
    logger->setFormat(false);
    logger->setLevel(Logger::LOG_ERROR);
}

void MainWindow::initConfig(const std::string &inifile) {
    ini_ = std::make_shared<IniFile>();
    if (ini_->exists(inifile)) {
        ini_->load(inifile);
    } else {
        ini_->set("system", "connect_pool", 10);
        ini_->set("system", "thread_count", 8);
        ini_->set("system", "table_count", 5);

        ini_->set("mysql", "host", "127.0.0.1");
        ini_->set("mysql", "port", 3306);
        ini_->set("mysql", "username", "root");
        ini_->set("mysql", "password", "123456");
        ini_->set("mysql", "data_database", "xh_data_server");
        ini_->set("mysql", "print_database", "xh_print_data");

        ini_->save(inifile);
    }
}

bool MainWindow::initConnectionPool() {

    auto mysql = (*ini_)["mysql"];

    // 检查数据库连接
    if (!checkDatabaseConnected(mysql)) {
        QMessageBox::critical(this, "警告", "远程数据库配置不正确，请检查配置，详情见日志 'error.log'.");
        exit(-2);
    }

    // 创建数据服务数据库连接池
    data_pool_ = std::make_shared<ConnectionPool>();
    data_pool_->size((*ini_)["system"]["connect_pool"]);
    data_pool_->create(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["data_database"], "utf8", true);

    // 创建打印数据数据库连接池
    print_pool_ = std::make_shared<ConnectionPool>();
    print_pool_->size((*ini_)["system"]["connect_pool"]);
    print_pool_->create(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["print_database"], "utf8", true);

    return true;
}

bool MainWindow::checkDatabaseConnected(std::map<std::string, Value> mysql) {
    zel::myorm::Database db;
    bool                 is_connected = db.connect(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["data_database"]);
    is_connected                      = db.connect(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["print_database"]);
    db.close();
    return is_connected;
}

void MainWindow::initUI() {
    ui_->result_group_box->setVisible(false);
    ui_->not_found_label->setVisible(false);

    auto conn   = print_pool_->get();
    auto orders = conn->tables();
    print_pool_->put(conn);

    for (auto order : orders) {
        ui_->order_id_box->addItem(QString(order.c_str()));
    }
}

bool MainWindow::download() {
    bool is_download = true;
    auto conn        = print_pool_->get();
    if (conn->table_exists(order_id_)) return false;
    return is_download;
}

bool MainWindow::query() {
    auto conn        = print_pool_->get();
    auto order_table = OrderTable(conn);
    order_table.table(order_id_);
    auto records = order_table.where("PUK1", "=", card_info_).all();
    print_pool_->put(conn);

    if (records.size() != 1) {
        ui_->not_found_label->setVisible(true);
        return false;
    }

    for (auto record : records) {
        std::string filename = record("datafile");
        std::string iccid    = record("ICCID");
        std::string puk      = record("PUK1");
        ui_->filename_line->setText(QString(filename.c_str()));
        ui_->iccid_line->setText(QString(iccid.c_str()));
        ui_->puk1_line->setText(QString(puk.c_str()));
        ui_->result_group_box->setVisible(true);
    }
    return true;
}