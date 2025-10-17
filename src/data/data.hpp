#pragma once

#include <memory>
#include <zel/utility/ini.h>

#include "myorm/connection_pool.h"

struct Data {
    std::shared_ptr<zel::utility::Ini>          ini;
    std::shared_ptr<zel::myorm::ConnectionPool> data_pool;
    std::shared_ptr<zel::myorm::ConnectionPool> print_pool;
    std::map<std::string, std::vector<int>>     configs;
    std::vector<std::string>                    files;
    std::string                                 file;
    std::string                                 order_no;
    std::string                                 card_info;
};
