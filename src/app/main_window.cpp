#include "main_window.h"
#include "model/xh_prjcfg.hpp"

#include "task/order.h"

#include <zel/thread.h>
using namespace zel::thread;

#include <zel/utility.h>
using namespace zel::utility;

#include <chrono>
#include <cstdlib>
#include <qmessagebox.h>
#include <qmessagebox>
#include <vector>

MainWindow::MainWindow(QMainWindow *parent)
    : QMainWindow(parent)
    , ui_(new Ui_MainWindow) {
    ui_->setupUi(this);

    initWindow();

    initUI();

    initSignalSlot();

    initConfig("./bin/config.ini");

    if (!initConnectionPool()) {
        QMessageBox::critical(this, "警告", "数据库配置不正确，请检查配置，详情见日志 'mysql.log'.");
        exit(-2);
    }
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::queryBtnClicked() {
    auto order_id  = ui_->order_id_line->text().toStdString();
    auto card_info = ui_->card_info_line->text().toStdString();

    if (order_id == "" || card_info == "") {
        QMessageBox::critical(this, "警告", "订单号或卡信息为空");
        return;
    }

    auto data_files = getOrderData(order_id);

    int thread_count = 10;
    int task_count   = data_files.size();

    auto logger = Logger::instance();
    if (logger->isOpen()) {
        printf("日志文件打开了\n");
    }

    // 多线程任务分发器初始化
    auto task_dispatcher = Singleton<TaskDispatcher>::instance();
    task_dispatcher->init(thread_count);

    // 计时开始
    auto start = std::chrono::system_clock::now();

    // 创建线程任务
    for (int i = 0; i < task_count; i++) {
        Task *task = new Order(&pool_, data_config_, data_files[i], card_info);
        task_dispatcher->assign(task);
    }
    // 等待任务完成
    task_dispatcher->wait();

    // 计时结束
    auto end = std::chrono::system_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("all task done, use time: %lld ms\n", time);
}

std::vector<std::string> MainWindow::getOrderData(std::string order_id) {
    std::vector<std::string> data_files;

    auto conn = pool_.get();
    auto all  = XhPrjcfg(conn).where("PrjId", "=", order_id).all();
    pool_.put(conn);

    data_config_ = all[0]("DataCfgA").asString();
    for (auto one : all) {
        std::string data_file = one("DataFiles");

        if (data_file.find(".prd") == std::string::npos) {
            continue;
        }

        String::toLower(data_file);
        data_file = "`" + data_file + "`";

        data_files.push_back(data_file);
    }

    return data_files;
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

bool MainWindow::initConnectionPool() {

    auto logger = zel::utility::Logger::instance();
    logger->open("./bin/mysql.log");
    logger->setFormat(false);

    // 检查数据库连接
    if (!checkDatabaseConnected()) {
        return false;
    }

    // 创建数据库连接池
    pool_.size(10);
    pool_.create(ini_["mysql"]["host"], ini_["mysql"]["port"], ini_["mysql"]["username"], ini_["mysql"]["password"], ini_["mysql"]["database"], "utf8", true);

    return true;

    // auto conn = pool.get();

    // // 添加一条数据
    // auto student    = Student(conn);
    // student["name"] = "jack";
    // student["age"]  = 18;
    // student["sex"]  = "男";
    // student.save();

    // pool.put(conn);
}

bool MainWindow::checkDatabaseConnected() {
    zel::myorm::Database db;
    bool                 is_connected =
        db.connect(ini_["mysql"]["host"], ini_["mysql"]["port"], ini_["mysql"]["username"], ini_["mysql"]["password"], ini_["mysql"]["database"]);
    db.close();
    return is_connected;
}