#pragma once

#include <memory>
#include <zel/myorm.h>
#include <zel/utility.h>

struct Data {
    std::shared_ptr<zel::utility::IniFile>      ini;
    std::shared_ptr<zel::myorm::ConnectionPool> data_pool;
    std::shared_ptr<zel::myorm::ConnectionPool> print_pool;
    std::map<std::string, std::vector<int>>     configs;
    std::vector<std::string>                    files;
    std::string                                 file;
    std::string                                 order_id;
    std::string                                 card_info;
};
