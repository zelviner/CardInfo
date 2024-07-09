#include <vector>
#pragma one

#include <string>
#include <zel/myorm.h>
#include <zel/thread.h>

class Order : public zel::thread::Task {

  public:
    Order(zel::myorm::ConnectionPool *pool, const std::string &data_config_, const std::string &data_files, const std::string &card_info);
    ~Order();

    void run() override;

    void destroy() override;

  private:
    void query();

    // void getOrderData();

    void getPrdData(const std::string &prd_file);

    bool getDataInfo(const std::string &data);

  private:
    zel::myorm::ConnectionPool *pool_;

    std::string data_config_;
    std::string data_files_;
    std::string card_info_;
};
