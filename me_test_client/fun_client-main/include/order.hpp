/*
 * Order.h
 *
 */

#pragma once

#include <string>
#include <types.hpp>
#include <utility>
using namespace std;
using namespace fun::client;

namespace fun::Matching {

/**
 * Order Type
 * e.g. OrderId,Symbol,Price,Volume,Time,Buy/Sell
 * e.g. 70000001,A,7321,100,100001,B
 * */
typedef struct Order {
    Xoid m_id;
    Price m_price;
    Volume m_quantity;
    bool m_isBuy;
    Time m_time;
    std::string m_name;

    Order(Xoid id, std::string name, Price price, Volume quantity, Time time, bool isBuy)
        : m_id(id), m_price(price), m_quantity(quantity), m_isBuy(isBuy), m_time(time), m_name(std::move(name)) {}
} Order;

inline ostream &operator<<(ostream &out, const Order &order) {
    out << order.m_name << "_" << order.m_price << "_" << order.m_quantity << "_"
        << "_" << order.m_time << "_" << order.m_isBuy;
    return out;
}

inline bool operator==(const Order &lhs, const Order &rhs) {
    return lhs.m_id == rhs.m_id && lhs.m_name == rhs.m_name && lhs.m_price == rhs.m_price &&
           lhs.m_quantity == rhs.m_quantity && lhs.m_time == rhs.m_time && lhs.m_isBuy == rhs.m_isBuy;
}

inline bool operator!=(const Order &lhs, const Order &rhs) { return !(lhs == rhs); }

}  // namespace fun::Matching
