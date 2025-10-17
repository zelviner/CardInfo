#pragma one

#include "data/data.hpp"

#include <qmainwindow.h>
#include <zel/thread/task_dispatcher.h>


class Order : public zel::thread::Task {
  public:
    Order();
    Order(Data *data);
    ~Order();

    void run() override;

    void destroy() override;
};
