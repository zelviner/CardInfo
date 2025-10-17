#pragma once

#include <memory>
#include <zel/utility/ini.h>

#include "myorm/connection_pool.h"

struct CardInfo {
    std::string file_name;
    std::string iccid;
    std::string puk;
};
