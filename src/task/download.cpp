#include "download.h"
#include "task/order.h"

#include <qobjectdefs.h>
#include <zel/thread/task_dispatcher.h>
#include <zel/utility/singleton.hpp>

using namespace zel::thread;

Download::Download(std::shared_ptr<Data> data)
    : data_(data) {}

Download::~Download() {}

void Download::run() {

    if (!create_order_table()) {
        return;
    }

    download();

    emit success();
}

bool Download::create_order_table() {

    auto conn = data_->print_pool->get();

    // 创建表结构
    std::string sql = "CREATE TABLE IF NOT EXISTS `" + data_->order_no + "`(\n";
    sql += "`id` int(10) NOT NULL AUTO_INCREMENT,\n`datafile` varchar(500) DEFAULT NULL,\n";
    for (auto data : data_->configs) {
        sql += "`" + data.first + "`" + "varchar(255) DEFAULT NULL,\n";
    }
    sql += "PRIMARY KEY (`ID`) USING BTREE\n) ENGINE=InnoDB AUTO_INCREMENT=127 DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC";

    if (!conn->execute(sql)) {
        data_->print_pool->put(conn);
        log_error("failed to create table `%s`", data_->order_no.c_str());
        return false;
    }
    data_->print_pool->put(conn);

    return true;
}

bool Download::download() {
    int thread_count = (*data_->ini)["system"]["thread_count"];
    int task_count   = data_->files.size();

    // 多线程任务分发器初始化
    auto task_dispatcher = Singleton<TaskDispatcher>::instance();
    task_dispatcher->init(thread_count);

    // 计时开始
    auto start = std::chrono::system_clock::now();

    // // 创建线程任务
    // for (int i = 0; i < task_count; i++) {
    //     Data *data       = new Data;
    //     data->data_pool  = data_->data_pool;
    //     data->print_pool = data_->print_pool;
    //     data->configs    = data_->configs;
    //     data->file       = data_->files[i];
    //     data->order_no   = data_->order_no;

    //     Task *task = new Order();
    //     task->data(data);

    //     task_dispatcher->assign(task);
    // }

    // 等待任务完成并获取完成数
    while (true) {
        double down = double(task_dispatcher->taskDownCount()) / task_count * 100;
        emit   threadDown(int(down));

        if (task_dispatcher->taskDownCount() == task_count) break;
    }

    task_dispatcher->taskDownCount(0);

    // 计时结束
    auto end = std::chrono::system_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    printf("all task done, use time: %lld ms\n", time);

    return true;
}
