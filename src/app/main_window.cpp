#include "main_window.h"

#include "order/order.h"

#include <qmessagebox.h>
#include <qmessagebox>

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow) {
    ui_->setupUi(this);

    initWindow();

    initUI();

    initSignalSlot();

    initConfig("./bin/config.ini");

    initConnectionPool();
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::queryBtnClicked() {
    auto order_id  = ui_->order_id_line->text().toStdString();
    auto card_info = ui_->card_info_line->text().toStdString();

    if (order_id == "" || card_info == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
    }

    Order order(&pool_, order_id, card_info);
    order.query();
}

void MainWindow::initWindow() {
    // 设置窗口标题
    setWindowTitle("星汉卡片信息");
}

void MainWindow::initUI() {
    // // 插入图片
    // QPixmap pixmap(":/image/data.png");
    // ui_->push_btn->setIcon(pixmap);
    // ui_->push_btn->setIconSize(pixmap.size());
    // ui_->push_btn->setFixedSize(pixmap.size());
}

void MainWindow::initSignalSlot() { connect(ui_->query_btn, &QPushButton::clicked, this, &MainWindow::queryBtnClicked); }

void MainWindow::initConfig(const std::string &inifile) {
    if (ini_.exists(inifile)) {
        ini_.load(inifile);
    } else {
        ini_.set("mysql", "host", "127.0.0.1");
        ini_.set("mysql", "port", 3306);
        ini_.set("mysql", "username", "root");
        ini_.set("mysql", "password", "123456");
        ini_.set("mysql", "database", "xh_data_server");

        ini_.save(inifile);
    }
}

void MainWindow::initConnectionPool() {

    zel::utility::Logger::instance()->open("./bin/mysql.log");

    pool_.size(3);
    pool_.create(ini_["mysql"]["host"], ini_["mysql"]["port"], ini_["mysql"]["username"], ini_["mysql"]["password"], ini_["mysql"]["database"], "utf8", true);

    // auto conn = pool.get();

    // // 添加一条数据
    // auto student    = Student(conn);
    // student["name"] = "jack";
    // student["age"]  = 18;
    // student["sex"]  = "男";
    // student.save();

    // pool.put(conn);
}