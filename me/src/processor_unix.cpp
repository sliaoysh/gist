#include "processor_unix.hpp"

void processor::run()
{
    trades_.reserve(64);
    soc_.accept_conn();

    while (true)
    {
        trades_.clear();
        auto* buf = soc_.recv_msg();
        const auto msg_type = buf[1];

        switch (msg_type)
        {
            case static_cast<char>(fun::client::MessageType::addOrder):
            {
                const fun::client::AddOrder* add_order = reinterpret_cast<fun::client::AddOrder*>(buf);
                const auto ex_order_id = me_.add_order(add_order->instrumentId, add_order->side, add_order->price, add_order->volume, trades_);

                for (auto& trade : trades_)
                {
                    soc_.send_msg(reinterpret_cast<const char*>(&trade), sizeof(fun::client::Trade));
                    std::swap(trade.orderId, trade.counterOrderId);
                    soc_.send_msg(reinterpret_cast<const char*>(&trade), sizeof(fun::client::Trade));
                }

                const fun::client::AddOrderAccepted reply{ex_order_id};
                soc_.send_msg(reinterpret_cast<const char*>(&reply), sizeof(fun::client::AddOrderAccepted));
                break;
            }
            case static_cast<char>(fun::client::MessageType::cancelOrder):
            {
                const fun::client::CancelOrder* cancel_order = reinterpret_cast<fun::client::CancelOrder*>(buf);
                const bool res = me_.cancel_order(cancel_order->exchangeOrderId);
                if (res)
                {
                    const fun::client::CancelOrderAccepted reply{cancel_order->exchangeOrderId};
                    soc_.send_msg(reinterpret_cast<const char*>(&reply), sizeof(fun::client::CancelOrderAccepted));
                }
                else
                {
                    const fun::client::CancelOrderRejected reply{cancel_order->exchangeOrderId};
                    soc_.send_msg(reinterpret_cast<const char*>(&reply), sizeof(fun::client::CancelOrderRejected));
                }
                break;
            }
        }
    }
}

processor::processor(const char* sock_name, const std::vector<fun::client::InstrumentId>& instruments)
    : me_{instruments}, soc_{sock_name}
{
}
