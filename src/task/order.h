#pragma one

#include "app/data.hpp"

#include <zel/myorm.h>
#include <zel/thread.h>
#include <zel/utility.h>

#include <qmainwindow.h>

class Order : public zel::thread::Task {
  public:
    Order();
    Order(Data *data);
    ~Order();

    void run() override;

    void destroy() override;
};
