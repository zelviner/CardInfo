#include "download.h"
#include "model/xh_prjcfg.hpp"
#include "task/order.h"

#include <zel/thread.h>
using namespace zel::thread;

Download::Download(std::shared_ptr<zel::utility::IniFile> ini, std::shared_ptr<zel::myorm::ConnectionPool> remote_pool,
                   std::shared_ptr<zel::myorm::ConnectionPool> local_pool, const std::string &order_id, const std::string &card_info,
                   std::vector<std::vector<std::string>> &data_files, std::map<std::string, std::vector<int>> &data_configs)
    : ini_(ini)
    , remote_pool_(remote_pool)
    , local_pool_(local_pool)
    , order_id_(order_id)
    , card_info_(card_info)
    , data_files_(data_files)
    , data_configs_(data_configs) {}

Download::~Download() {}

void Download::run() {

    if (!createOrderTable()) {
        return;
    }

    download();

    emit success();
}

std::vector<std::vector<std::string>> Download::getOrderData() {
    std::vector<std::vector<std::string>> data_files;

    auto conn = remote_pool_->get();
    auto all  = XhPrjcfg(conn).where("PrjId", "=", order_id_).all();
    remote_pool_->put(conn);
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

bool Download::createOrderTable() {

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

bool Download::download() {
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

    while (true) {
        double down = double(task_dispatcher->down()) / task_count * 100;
        emit   threadDown(int(down));

        if (down == task_count) break;
    }

    // 计时结束
    auto end = std::chrono::system_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("all task done, use time: %lld ms\n", time);

    return true;
}
