#include "main_window.h"
#include "model/xh_prjcfg.hpp"
#include "task/order.h"
#include "task/query.h"

#include <cstdlib>
#include <qmessagebox.h>
#include <qmessagebox>
#include <qmovie>
#include <qpushbutton.h>
#include <qvariant.h>
#include <vector>

using namespace zel::thread;
using namespace zel::utility;
using namespace zel::myorm;

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow)
    , download_(nullptr)
    , download_loading_(nullptr)
    , data_(std::make_shared<Data>()) {
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
    data_->order_id  = ui_->order_id_box->currentText().toStdString();
    data_->card_info = ui_->card_info_line->text().toStdString();
    ui_->result_group_box->hide();
    ui_->not_found_label->hide();

    if (data_->order_id == "" || data_->card_info == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
        return;
    }

    data_->files = dataFiles();

    if (data_->files.size() == 0) {
        QMessageBox::critical(this, "警告", "未找到该订单，请检查订单号是否正确");
        return;
    }

    // 查询订单数据是否存在
    if (orderDataIsExist()) {
        // 弹出下载加载窗口, 并停留
        if (download_loading_ == nullptr) {
            download_loading_ = new DownloadLoading(this);
        }
        download_loading_->show();

        if (download_ == nullptr) {
            download_ = new Download(data_);
        }

        // 连接信号槽
        connect(download_, &Download::success, this, &MainWindow::success);
        connect(download_, &Download::threadDown, download_loading_, &DownloadLoading::threadDown);

        // 启动工作线程
        download_->start();
    } else {
        // 查询
        query();
    }
}

void MainWindow::deleteBtnClicked() {

    auto order_id = ui_->order_id_box->currentText();

    auto ques = QMessageBox::question(this, "提示", "确定删除订单 " + order_id + " 吗？", QMessageBox::Yes | QMessageBox::No);

    if (ques == QMessageBox::No) return;

    auto        conn = data_->print_pool->get();
    Database    db(conn);
    std::string sql = "drop table if exists `" + order_id.toStdString() + "`";
    db.execute(sql);
    data_->print_pool->put(conn);
}

void MainWindow::saveBtnClicked() {
    data_->ini->set("system", "connect_count", ui_->connect_count_spin_box->value());
    data_->ini->set("system", "thread_count", ui_->thread_count_spin_box->value());

    data_->ini->set("mysql", "host", ui_->host_line->text().toStdString());
    data_->ini->set("mysql", "port", ui_->port_line->text().toStdString());
    data_->ini->set("mysql", "username", ui_->username_line->text().toStdString());
    data_->ini->set("mysql", "password", ui_->password_line->text().toStdString());
    data_->ini->set("mysql", "data_database", ui_->data_database_line->text().toStdString());
    data_->ini->set("mysql", "print_database", ui_->print_database_line->text().toStdString());

    data_->ini->save("config.ini");

    QMessageBox::information(this, "提示", "设置保存成功");
}

void MainWindow::success() {

    if (download_loading_ != nullptr) {
        delete download_loading_;
        download_loading_ = nullptr;
    }

    download_->destroyed();
    download_ = nullptr;

    printf("success\n");

    query();
}

void MainWindow::failure() {}

void MainWindow::notFound() {
    ui_->query_label->hide();
    ui_->query_gif_label->hide();
    ui_->not_found_label->show();
}

void MainWindow::showResult(const QString &filename, const QString &iccid, const QString &puk) {
    ui_->query_label->hide();
    ui_->query_gif_label->hide();
    ui_->filename_line->setText(filename);
    ui_->iccid_line->setText(iccid);
    ui_->puk1_line->setText(puk);
    ui_->result_group_box->show();
}

void MainWindow::initWindow() {
    // 设置窗口标题
    setWindowTitle("查询卡片信息");
}

void MainWindow::initSignalSlot() {
    connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked);
    connect(ui_->delete_btn, &QPushButton::clicked, this, &MainWindow::deleteBtnClicked);
    connect(ui_->save_btn, &QPushButton::clicked, this, &MainWindow::saveBtnClicked);
}

void MainWindow::initLogger() {
    auto logger = Logger::instance();
    logger->open("error.log");
    logger->setFormat(false);
    logger->setLevel(Logger::LOG_ERROR);
}

void MainWindow::initConfig(const std::string &inifile) {
    data_->ini = std::make_shared<IniFile>();
    if (data_->ini->exists(inifile)) {
        data_->ini->load(inifile);
    } else {
        data_->ini->set("system", "connect_count", 10);
        data_->ini->set("system", "thread_count", 8);

        data_->ini->set("mysql", "host", "127.0.0.1");
        data_->ini->set("mysql", "port", 3306);
        data_->ini->set("mysql", "username", "root");
        data_->ini->set("mysql", "password", "123456");
        data_->ini->set("mysql", "data_database", "xh_data_server");
        data_->ini->set("mysql", "print_database", "xh_print_data");

        data_->ini->save(inifile);
    }
}

bool MainWindow::initConnectionPool() {

    auto mysql = (*data_->ini)["mysql"];

    // 检查数据库连接
    if (!checkDatabaseConnected(mysql)) {
        QMessageBox::critical(this, "警告", "远程数据库配置不正确，请检查配置，详情见日志 'error.log'.");
        exit(-2);
    }

    // 创建数据服务数据库连接池
    data_->data_pool = std::make_shared<ConnectionPool>();
    data_->data_pool->size((*data_->ini)["system"]["connect_count"]);
    data_->data_pool->create(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["data_database"], "utf8", true);

    // 创建打印数据数据库连接池
    data_->print_pool = std::make_shared<ConnectionPool>();
    data_->print_pool->size((*data_->ini)["system"]["connect_count"]);
    data_->print_pool->create(mysql["host"], mysql["port"], mysql["username"], mysql["password"], mysql["print_database"], "utf8", true);

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

    // 设置查询加载动画
    ui_->query_gif_label->setStyleSheet("background-color: transparent;");
    auto movie = new QMovie(":/images/loading.gif");
    movie->setScaledSize(QSize(32, 32));
    ui_->query_gif_label->setMovie(movie);
    ui_->query_gif_label->setScaledContents(true);
    movie->start();

    // 初始化 设置 tab
    // ui_->host_line->

    auto mysql_ini = (*data_->ini)["mysql"];
    ui_->host_line->setText(QString(mysql_ini["host"].asString().c_str()));
    ui_->port_line->setText(QString(mysql_ini["port"].asString().c_str()));
    ui_->username_line->setText(QString(mysql_ini["username"].asString().c_str()));
    ui_->password_line->setText(QString(mysql_ini["password"].asString().c_str()));
    ui_->data_database_line->setText(QString(mysql_ini["data_database"].asString().c_str()));
    ui_->print_database_line->setText(QString(mysql_ini["print_database"].asString().c_str()));

    auto system_ini = (*data_->ini)["system"];
    ui_->connect_count_spin_box->setValue(system_ini["connect_count"]);
    ui_->thread_count_spin_box->setValue(system_ini["thread_count"]);

    ui_->result_group_box->hide();
    ui_->not_found_label->hide();
    ui_->query_label->hide();
    ui_->query_gif_label->hide();

    auto conn   = data_->print_pool->get();
    auto orders = conn->tables();
    data_->print_pool->put(conn);

    for (auto order : orders) {
        String::toUpper(order);
        ui_->order_id_box->addItem(QString(order.c_str()));
    }
}

bool MainWindow::orderDataIsExist() {
    bool is_download = true;
    auto conn        = data_->print_pool->get();
    if (conn->table_exists(data_->order_id)) return false;
    data_->print_pool->put(conn);
    return is_download;
}

std::vector<std::string> MainWindow::dataFiles() {
    std::vector<std::string> data_files;

    auto conn = data_->data_pool->get();
    auto all  = XhPrjcfg(conn).where("PrjId", "=", data_->order_id).all();
    data_->data_pool->put(conn);
    std::string data_config = all[0]("DataCfgA").asString();

    data_->configs = dataConfig(data_config);

    for (int i = 0; i < all.size(); i++) {
        std::string data_file = all[i]("DataFiles");

        if (data_file.find(".prd") == std::string::npos) {
            continue;
        }

        String::toLower(data_file);
        data_files.push_back(data_file);
    }

    return data_files;
}

std::map<std::string, std::vector<int>> MainWindow::dataConfig(const std::string &data_config) {

    std::map<std::string, std::vector<int>> data_indexs;

    auto        data_configs = String::split(data_config, ",");
    std::string header, data = "";
    int         index  = 0;
    int         length = 0;

    for (auto config : data_configs) {
        std::vector<int> indexs;
        header = config.substr(0, config.find(":"));
        length = stoi(config.substr(config.find(":") + 1));

        indexs.push_back(index);
        indexs.push_back(length);

        index += length;

        int interception = header.find("PRINT");
        if (interception != std::string::npos) {
            data_indexs[header.substr(interception + 6)] = indexs;
        }
    }

    return data_indexs;
}

bool MainWindow::query() {

    ui_->query_label->show();
    ui_->query_gif_label->show();

    Query *query = new Query(data_);
    // 连接信号槽
    connect(query, &Query::showResult, this, &MainWindow::showResult);
    connect(query, &Query::notFound, this, &MainWindow::notFound);

    // 启动工作线程
    query->start();

    return true;
}
