#include "main_window.h"
#include "download_loading.h"
#include "model/order_table.hpp"
#include "model/xh_prjcfg.hpp"
#include "task/download.h"
#include "task/order.h"

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
        auto download = new Download(ini_, remote_pool_, local_pool_, order_id_, card_info_, data_files_, data_configs_);

        // 连接信号槽
        connect(download, &Download::success, this, &MainWindow::success);

        // 启动工作线程
        download->start();
    } else {
        // 查询
        query();
    }
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

    auto conn = remote_pool_->get();
    auto all  = XhPrjcfg(conn).where("PrjId", "=", order_id_).all();
    remote_pool_->put(conn);

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

bool MainWindow::downloadData() {
    int thread_count = (*ini_)["system"]["thread_count"];
    int task_count   = data_files_.size();

    // 多线程任务分发器初始化
    auto task_dispatcher = Singleton<TaskDispatcher>::instance();
    task_dispatcher->init(thread_count);

    // 计时开始
    auto start = std::chrono::system_clock::now();

    // 创建线程任务
    for (int i = 0; i < task_count; i++) {
        Task *task = new Order(remote_pool_, local_pool_, data_configs_, data_files_[i], card_info_, order_id_);
        task_dispatcher->assign(task);
    }
    // 等待任务完成
    task_dispatcher->wait();

    // 计时结束
    auto end = std::chrono::system_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("all task done, use time: %lld ms\n", time);

    return true;
}

void MainWindow::initWindow() {
    // 设置窗口标题
    setWindowTitle("星汉卡片信息");
}

void MainWindow::initSignalSlot() { connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked); }

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

        ini_->set("remote_mysql", "host", "127.0.0.1");
        ini_->set("remote_mysql", "port", 3306);
        ini_->set("remote_mysql", "username", "root");
        ini_->set("remote_mysql", "password", "123456");
        ini_->set("remote_mysql", "database", "xh_data_server");

        ini_->set("local_mysql", "host", "127.0.0.1");
        ini_->set("local_mysql", "port", 3306);
        ini_->set("local_mysql", "username", "root");
        ini_->set("local_mysql", "password", "123456");
        ini_->set("local_mysql", "database", "xh_data_server");

        ini_->save(inifile);
    }
}

bool MainWindow::initConnectionPool() {

    auto remote_mysql = (*ini_)["remote_mysql"];
    auto local_mysql  = (*ini_)["local_mysql"];

    // 检查远程数据库连接
    if (!checkDatabaseConnected(remote_mysql)) {
        QMessageBox::critical(this, "警告", "远程数据库配置不正确，请检查配置，详情见日志 'mysql.log'.");
        exit(-2);
    }

    // 检查远程数据库连接
    if (!checkDatabaseConnected(local_mysql)) {
        QMessageBox::critical(this, "警告", "本地数据库配置不正确，请检查配置，详情见日志 'mysql.log'.");
        exit(-2);
    }

    // 创建远程数据库连接池
    remote_pool_ = std::make_shared<ConnectionPool>();
    remote_pool_->size((*ini_)["system"]["connect_pool"]);
    remote_pool_->create(remote_mysql["host"], remote_mysql["port"], remote_mysql["username"], remote_mysql["password"], remote_mysql["database"], "utf8",
                         true);

    // 创建本地数据库连接池
    local_pool_ = std::make_shared<ConnectionPool>();
    local_pool_->size((*ini_)["system"]["connect_pool"]);
    local_pool_->create(local_mysql["host"], local_mysql["port"], local_mysql["username"], local_mysql["password"], local_mysql["database"], "utf8", true);

    return true;
}

bool MainWindow::checkDatabaseConnected(std::map<std::string, Value> mysql) {
    zel::myorm::Database db;
    bool                 is_connected = db.connect(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["database"]);
    db.close();
    return is_connected;
}

void MainWindow::initUI() {
    ui_->result_group_box->setVisible(false);
    ui_->not_found_label->setVisible(false);

    auto conn   = local_pool_->get();
    auto orders = conn->tables();
    local_pool_->put(conn);

    for (auto order : orders) {
        ui_->order_id_box->addItem(QString(order.c_str()));
    }
}

bool MainWindow::download() {
    bool is_download = true;
    auto conn        = local_pool_->get();
    if (conn->table_exists(order_id_)) return false;
    return is_download;
}

bool MainWindow::createOrderTable() {

    auto conn = local_pool_->get();

    // 创建表结构
    std::string sql = "CREATE TABLE IF NOT EXISTS `" + order_id_ + "`(\n";
    sql += "`id` int(10) NOT NULL AUTO_INCREMENT,\n`datafile` varchar(500) DEFAULT NULL,\n";
    for (auto data : data_configs_) {
        sql += "`" + data.first + "`" + "varchar(255) DEFAULT NULL,\n";
    }
    sql += "PRIMARY KEY (`ID`) USING BTREE\n) ENGINE=InnoDB AUTO_INCREMENT=127 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC";

    printf("%s\n", sql.c_str());

    if (!conn->execute(sql)) {
        log_error("failed to create table `%s`", order_id_.c_str());
        return false;
    }

    return true;
}

bool MainWindow::query() {
    auto conn        = local_pool_->get();
    auto order_table = OrderTable(conn);
    order_table.table(order_id_);
    auto records = order_table.where("PUK1", "=", card_info_).all();
    local_pool_->put(conn);

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