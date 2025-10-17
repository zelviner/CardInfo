#pragma once

#include "data/data.hpp"

#include <memory>
#include <qthread>

class Query : public QThread {
    Q_OBJECT
  public:
    Query(std::shared_ptr<Data> data);

    ~Query();

    void run() override;

  signals:
    void notFound();
    void showResult(const QString &filename, const QString &iccid, const QString &puk);

  private:
    std::shared_ptr<Data> data_;
};