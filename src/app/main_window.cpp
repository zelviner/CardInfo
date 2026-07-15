#include "main_window.h"
#include "order/order.h"
#include "task/query.h"

#include <algorithm>
#include <memory>
#include <qmessagebox.h>
#include <qmessagebox>
#include <qmovie>
#include <qpushbutton.h>
#include <qvariant.h>
#include <vector>

using namespace zel::utility;
using namespace zel::myorm;

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow)
    , download_loading_(nullptr)
    , finished_count_(0)
    , active_thread_count_(0)
    , connection_count_(0)
    , query_found_(false)
    , query_failed_(false) {
    ui_->setupUi(this);

    init_window();

    init_signals_slots();

    init_logger();

    init_config("config.ini");

    bool connection_ready = init_connection_pool();

    init_ui();

    ui_->query_btn->setEnabled(connection_ready);

    qRegisterMetaType<std::shared_ptr<CardInfo>>("std::shared_ptr<CardInfo>");
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::queryBtnClicked() {
    if (active_thread_count_ > 0 && finished_count_ < active_thread_count_) {
        return;
    }

    std::string order_no  = ui_->order_no_box->currentText().toStdString();
    std::string card_info = ui_->card_info_line->text().toStdString();
    ui_->result_group_box->hide();
    ui_->not_found_label->hide();

    if (order_no == "" || card_info == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
        return;
    }

    auto conn = connection_pool_->get();
    if (conn == nullptr) {
        QMessageBox::critical(this, "错误", "没有可用的数据库连接");
        return;
    }

    // 查询订单数据是否存在
    Order order(conn);
    if (order.exists(order_no)) {
        int data_size = order.dataSize(order_no);
        connection_pool_->put(conn);
        if (data_size == 0) {
            ui_->card_info_line->selectAll();
            ui_->card_info_line->setFocus();
            ui_->query_label->hide();
            ui_->query_gif_label->hide();
            ui_->not_found_label->show();
            return;
        }

        int thread_count = (std::min)({ui_->thread_count_spin_box->value(), connection_count_, data_size});
        if (thread_count <= 0) {
            QMessageBox::critical(this, "错误", "查询线程数必须大于 0");
            return;
        }

        finished_count_      = 0;
        active_thread_count_ = thread_count;
        query_found_         = false;
        query_failed_        = false;
        ui_->query_btn->setEnabled(false);
        ui_->card_info_line->setEnabled(false);
        ui_->query_label->show();
        ui_->query_gif_label->show();

        int data_per_thread = data_size / thread_count;
        int data_left       = data_size % thread_count;
        for (int i = 0; i < thread_count; i++) {
            int start_id = i * data_per_thread + (i < data_left ? i : data_left) + 1;
            int end_id   = start_id + data_per_thread + (i < data_left ? 1 : 0);

            Query *query = new Query(connection_pool_, order_no, card_info, start_id, end_id);
            // 连接信号槽
            connect(query, &Query::found, this, &MainWindow::found);
            connect(query, &Query::notFound, this, &MainWindow::notFound);
            connect(query, &Query::failed, this, &MainWindow::queryFailed);
            connect(query, &QThread::finished, query, &QObject::deleteLater);

            // 启动工作线程
            query->start();
        }
    } else {
        connection_pool_->put(conn);
        QMessageBox::critical(this, "警告", "未找到该订单，请检查订单号是否正确");
    }
}

void MainWindow::saveBtnClicked() {
    ini_.set("system", "connect_count", ui_->connect_count_spin_box->value());
    ini_.set("system", "thread_count", ui_->thread_count_spin_box->value());

    ini_.set("mysql", "host", ui_->host_line->text().toStdString());
    ini_.set("mysql", "port", ui_->port_line->text().toStdString());
    ini_.set("mysql", "username", ui_->username_line->text().toStdString());
    ini_.set("mysql", "password", ui_->password_line->text().toStdString());
    ini_.set("mysql", "database", ui_->database_line->text().toStdString());

    ini_.save("config.ini");

    QMessageBox::information(this, "提示", "设置保存成功");
}

void MainWindow::notFound() {
    finish_query_thread();
}

void MainWindow::found(std::shared_ptr<CardInfo> card_info) {
    if (!query_found_) {
        query_found_ = true;
        ui_->filename_line->setText(card_info->file_name.c_str());
        ui_->iccid_line->setText(card_info->iccid.c_str());
        ui_->puk1_line->setText(card_info->imsi.c_str());
        ui_->serial_number_line->setText(card_info->serial_number.c_str());
        ui_->box_number_line->setText(card_info->box_number.c_str());
        ui_->carton_number_line->setText(card_info->carton_number.c_str());
        ui_->result_group_box->show();
    }

    finish_query_thread();
}

void MainWindow::queryFailed(const QString &message) {
    if (!query_failed_) {
        query_failed_ = true;
        QMessageBox::critical(this, "查询失败", message);
    }

    finish_query_thread();
}

void MainWindow::init_window() {
    // 设置窗口标题
    setWindowTitle("查询卡片信息 v2.1.1");
}

void MainWindow::init_signals_slots() {
    connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked);
    connect(ui_->save_btn, &QPushButton::clicked, this, &MainWindow::saveBtnClicked);
    connect(ui_->card_info_line, &QLineEdit::returnPressed, this, &MainWindow::queryBtnClicked);
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
        ini_.set("mysql", "database", "if_dms");

        ini_.save(ini_file);
    }
}

bool MainWindow::init_connection_pool() {
    connection_pool_ = std::make_shared<zel::myorm::ConnectionPool>();
    connection_count_ = (std::max)(1, ini_["system"]["connect_count"].asInt());
    connection_pool_->size(connection_count_);
    connection_pool_->create(ini_["mysql"]["host"], ini_["mysql"]["port"], ini_["mysql"]["username"], ini_["mysql"]["password"], ini_["mysql"]["database"],
                             "utf8");

    auto conn = connection_pool_->get();

    if (conn == nullptr) {
        QMessageBox::critical(this, "错误", "数据库连接失败");
        return false;
    }

    Order order(conn);
    auto  orders = order.orders();

    for (auto order : orders) {
        ui_->order_no_box->addItem(QString::fromStdString(order));
    }

    connection_pool_->put(conn);
    return true;
}

void MainWindow::finish_query_thread() {
    finished_count_++;
    if (finished_count_ != active_thread_count_) {
        return;
    }

    ui_->query_btn->setEnabled(true);
    ui_->card_info_line->setEnabled(true);
    ui_->card_info_line->selectAll();
    ui_->card_info_line->setFocus();
    ui_->query_label->hide();
    ui_->query_gif_label->hide();
    if (!query_found_ && !query_failed_) {
        ui_->not_found_label->show();
    }
}

void MainWindow::init_ui() {

    // 设置查询加载动画
    ui_->query_gif_label->setStyleSheet("background-color: transparent;");
    auto movie = new QMovie(":/images/loading.gif");
    movie->setScaledSize(QSize(32, 32));
    ui_->query_gif_label->setMovie(movie);
    ui_->query_gif_label->setScaledContents(true);
    movie->start();

    auto mysql_ini = ini_["mysql"];
    ui_->host_line->setText(QString(mysql_ini["host"].asString().c_str()));
    ui_->port_line->setText(QString(mysql_ini["port"].asString().c_str()));
    ui_->username_line->setText(QString(mysql_ini["username"].asString().c_str()));
    ui_->password_line->setText(QString(mysql_ini["password"].asString().c_str()));
    ui_->database_line->setText(QString(mysql_ini["database"].asString().c_str()));

    auto system_ini = ini_["system"];
    ui_->connect_count_spin_box->setValue(system_ini["connect_count"]);
    ui_->thread_count_spin_box->setValue(system_ini["thread_count"]);

    ui_->result_group_box->hide();
    ui_->not_found_label->hide();
    ui_->query_label->hide();
    ui_->query_gif_label->hide();
}
