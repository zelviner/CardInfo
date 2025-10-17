#include "query.h"

#include "order/order.h"

Query::Query(std::shared_ptr<zel::myorm::ConnectionPool> connection_pool, const std::string &order_no, const std::string &card_no, int start_id, int end_id)
    : connection_pool_(connection_pool)
    , order_no_(order_no)
    , card_no_(card_no)
    , start_id_(start_id)
    , end_id_(end_id) {}

Query::~Query() {}

void Query::run() {
    auto conn = connection_pool_->get();

    Order order(conn);
    auto  card_info = order.query(order_no_, card_no_, start_id_, end_id_);

    if (card_info != nullptr) {
        emit found(card_info);
    } else {
        emit notFound();
    }

    connection_pool_->put(conn);
}