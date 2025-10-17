#include "main_window.h"
#include "myorm/database.h"
#include "order/order.h"
#include "task/order.h"
#include "task/query.h"

#include <memory>
#include <qmessagebox.h>
#include <qmessagebox>
#include <qmovie>
#include <qpushbutton.h>
#include <qvariant.h>
#include <vector>
#include <zel/utility/logger.h>
#include <zel/utility/string.h>

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

    init_window();

    init_signals_slots();

    init_logger();

    init_config("config.ini");

    init_database();

    init_ui();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::queryBtnClicked() {
    std::string order_no  = ui_->order_no_box->currentText().toStdString();
    std::string card_info = ui_->card_info_line->text().toStdString();
    ui_->result_group_box->hide();
    ui_->not_found_label->hide();

    if (order_no == "" || card_info == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
        return;
    }

    // 查询订单数据是否存在
    Order order(db_);
    if (order.exists(order_no)) {
        // 查询
        query();
    } else {
        QMessageBox::critical(this, "警告", "未找到该订单，请检查订单号是否正确");
    }
}

void MainWindow::deleteBtnClicked() {

    auto order_no = ui_->order_no_box->currentText();

    auto ques = QMessageBox::question(this, "提示", "确定删除订单 " + order_no + " 吗？", QMessageBox::Yes | QMessageBox::No);

    if (ques == QMessageBox::No) return;

    auto        conn = data_->print_pool->get();
    Database    db(conn);
    std::string sql = "drop table if exists `" + order_no.toStdString() + "`";
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

void MainWindow::init_window() {
    // 设置窗口标题
    setWindowTitle("查询卡片信息");
}

void MainWindow::init_signals_slots() {
    connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked);
    connect(ui_->delete_btn, &QPushButton::clicked, this, &MainWindow::deleteBtnClicked);
    connect(ui_->save_btn, &QPushButton::clicked, this, &MainWindow::saveBtnClicked);
}

void MainWindow::init_logger() {
    Logger::instance().open("error.log");
    Logger::instance().setFormat(false);
    Logger::instance().setLevel(Logger::LOG_ERROR);
}

void MainWindow::init_config(const std::string &ini_file) {
    if (ini_.exists(ini_file)) {
        ini_.load(ini_file);
    } else {
        ini_.set("system", "connect_count", 10);
        ini_.set("system", "thread_count", 8);

        ini_.set("mysql", "host", "127.0.0.1");
        ini_.set("mysql", "port", 3306);
        ini_.set("mysql", "username", "root");
        ini_.set("mysql", "password", "123456");
        ini_.set("mysql", "data_database", "if_dms");
        ini_.set("mysql", "print_database", "xh_print_data");

        ini_.save(ini_file);
    }
}

bool MainWindow::init_database() {
    db_ = std::make_shared<zel::myorm::Database>();
    if (!db_->connect(ini_["mysql"]["host"], ini_["mysql"]["port"], ini_["mysql"]["user"], ini_["mysql"]["password"], ini_["mysql"]["database"])) {
        QMessageBox::critical(this, "错误", "数据库连接失败");
        return false;
    }

    Order order(db_);
    auto  orders = order.orders();

    for (auto order : orders) {
        ui_->order_no_box->addItem(QString::fromStdString(order));
    }

    return true;
}

void MainWindow::init_ui() {

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
        ui_->order_no_box->addItem(QString(order.c_str()));
    }
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
