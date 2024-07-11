#include <memory>
#include <vector>
#pragma one

#include <string>
#include <zel/myorm.h>
#include <zel/thread.h>
#include <zel/utility.h>

class Order : public zel::thread::Task {

  public:
    Order(std::shared_ptr<zel::myorm::ConnectionPool> remote_pool, std::shared_ptr<zel::myorm::ConnectionPool> local_pool,
          std::map<std::string, std::vector<int>> &data_configs, std::vector<std::string> data_files, const std::string &card_info, const std::string &order_id);
    ~Order();

    void run() override;

    void destroy() override;

    static std::map<std::string, std::vector<int>> getDataIndex(const std::string &data_config);

  private:
    void getPrdData(std::vector<std::string> prd_file);

  private:
    std::shared_ptr<zel::myorm::ConnectionPool> remote_pool_;
    std::shared_ptr<zel::myorm::ConnectionPool> local_pool_;

    std::map<std::string, std::vector<int>> data_configs_;
    std::vector<std::string>                data_files_;
    std::string                             card_info_;
    std::string                             order_id_;
};
