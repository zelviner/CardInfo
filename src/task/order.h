#pragma one

#include "app/data.hpp"

#include <qmainwindow.h>
#include <zel/zel.h>


class Order : public zel::thread::Task {
  public:
    Order();
    Order(Data *data);
    ~Order();

    void run() override;

    void destroy() override;
};
