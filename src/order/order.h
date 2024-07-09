#include <vector>
#pragma one

#include <string>
#include <zel/myorm.h>

class Order {

  public:
    Order(zel::myorm::ConnectionPool *pool, const std::string &order_id, const std::string &card_info);
    ~Order();

    void query();

  private:
    void getOrderData();

    void getPrdData(const std::string &prd_file);

  private:
    zel::myorm::ConnectionPool *pool_;

    std::string              order_id_;
    std::string              card_info_;
    std::vector<std::string> data_files_;
    std::string              data_config_;
};
