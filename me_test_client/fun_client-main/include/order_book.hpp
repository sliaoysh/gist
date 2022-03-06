/*
 * OrderBook.h
 *
 *  */

#pragma once
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include "order.hpp"
using namespace std;
using namespace fun::client;

namespace fun::Matching {

#define INAN std::numeric_limits<Volume>::min()
#define IS_VALID(x) (x != INAN)

class PriceNode;

/**
 * Size  comparator
 *  i.e. order with larger quantity
 * */
struct OrderSizeComparator {
    bool operator()(const Order *a, const Order *b) const {
        if (a->m_quantity != b->m_quantity)
            return (a->m_quantity > b->m_quantity);
        else
            return a->m_time < b->m_time;
    }
};

typedef PriceNode *PriceNodePtr;
typedef map<Price, PriceNodePtr> PriceTree;
typedef map<Price, PriceNodePtr>::iterator PriceTreeIt;
typedef map<Price, PriceNodePtr>::reverse_iterator PriceTreeRevIt;
typedef unordered_map<Price, PriceNodePtr> PriceToNodeMap;
typedef unordered_map<Price, PriceNodePtr>::iterator PriceToNodeMapIt;
typedef set<Order *, OrderSizeComparator> OrderTree;
typedef set<Order *, OrderSizeComparator>::iterator OrderTreeIt;
typedef unordered_map<string, int> AccountMap;
typedef unordered_map<string, int>::iterator AccountMapIt;

/**
 * Node in bid or ask binary sorted tree in OrderBook
 *
 * */
class PriceNode {
private:
    Price m_price;
    set<Order *, OrderSizeComparator> *m_orderTree;

public:
    PriceNode() : m_price(INAN), m_orderTree(NULL) {}
    PriceNode(Price price) : m_price(price) { m_orderTree = new OrderTree(); }
    void clean();
    virtual ~PriceNode() { clean(); }

    Price getPrice() const { return m_price; }
    OrderTree *&getOrderTree() { return m_orderTree; }
    void insertOrder(Order *order) { m_orderTree->emplace(order); }

    friend ostream &operator<<(ostream &out, const PriceNode &priceNode);
};

inline ostream &operator<<(ostream &out, const PriceNode &priceNode) {
    out << "[ " << priceNode.getPrice() << ", <";
    for (auto node : *priceNode.m_orderTree) cout << *node << ",";
    out << "> ]";
    return out;
}

/**
 * Order book
 *
 * */
class OrderBook {
private:
    // binary sorted tree for indexing bid and ask orders within order book
    // PriceNode contains all quotes in size>time order using another BST for that price level
    map<Price, PriceNodePtr> *m_bidTree;  // < price, LimitPriceNode >
    map<Price, PriceNodePtr> *m_askTree;

    // hashmap to speed up add() in order book to O(1) if price already exists
    unordered_map<Price, PriceNodePtr> *m_bidMap;  // < price, LimitPriceNode >
    unordered_map<Price, PriceNodePtr> *m_askMap;

    // for booking trade
    unordered_map<string, int> *m_account;

public:
    OrderBook();
    virtual ~OrderBook();

    void add(Order *order);
    void match(const Order *order, Volume &qtyToMatch);
    void match(OrderTreeIt it, const Order *order, Volume &qtyToMatch, OrderTree *quotes, Price bestPrice);

    PriceTree *&getBidTree() { return m_bidTree; }
    PriceTree *&getAskTree() { return m_askTree; }

    PriceToNodeMap *&getAskMap() { return m_askMap; }
    PriceToNodeMap *&getBidMap() { return m_bidMap; }

    bool isMarketable(const Order *order, Price bestPrice, bool isBuy);

    void bookTrade(Volume execQty, const string &buyer, const string &seller);
    void bookTradeForInstrument(const vector<string> &names);
    int getTraderExposure(const string &name);

    friend ostream &operator<<(ostream &out, const OrderBook &book);
};

inline ostream &operator<<(ostream &out, const OrderBook &book) {
    out << "\n---ask---\n";
    for (PriceTreeRevIt it = book.m_askTree->rbegin(); it != book.m_askTree->rend(); ++it) {
        out << "price: " << it->first << " [ ";
        for (const auto &order : *(it->second->getOrderTree())) out << *order << " ";
        out << " ]" << endl;
    }
    out << "---bid---\n";
    for (PriceTreeRevIt it = book.m_bidTree->rbegin(); it != book.m_bidTree->rend(); ++it) {
        out << "price: " << it->first << " [ ";
        for (const auto &order : *(it->second->getOrderTree())) out << *order << " ";
        out << " ]" << endl;
    }
    return out;
}

//----------------------------------
// PriceNode
//----------------------------------

inline void PriceNode::clean() {
    for (OrderTreeIt it = m_orderTree->begin(); it != m_orderTree->end(); ++it) delete *it;
    delete m_orderTree;
}

//----------------------------------
// OrderBook
//----------------------------------

inline OrderBook::OrderBook() {
    m_bidTree = new PriceTree();
    m_askTree = new PriceTree();
    m_bidMap = new PriceToNodeMap();
    m_askMap = new PriceToNodeMap();
    m_account = new AccountMap();
}

inline OrderBook::~OrderBook() {
    for (PriceTreeIt it = m_bidTree->begin(); it != m_bidTree->end(); ++it) delete it->second;
    for (PriceTreeIt it = m_askTree->begin(); it != m_askTree->end(); ++it) delete it->second;

    delete m_bidTree;
    delete m_askTree;
    delete m_bidMap;
    delete m_askMap;
    delete m_account;
}

inline bool OrderBook::isMarketable(const Order *order, Price bestPrice, bool isBuy) {
    if (isBuy)
        return order->m_price >= bestPrice;
    else
        return order->m_price <= bestPrice;
}

/**
 * Marketable order handling:
 *  Remove liquidity to the other side of the book given and order
 *  time: O(1)
 * */
inline void OrderBook::match(const Order *order, Volume &qtyToMatch) {
    bool isBuy = order->m_isBuy;

    // get opposite side of tree to match
    if (isBuy) {
        for (PriceTreeIt itBestPrice = m_askTree->begin();
             itBestPrice != m_askTree->end() && isMarketable(order, m_askTree->begin()->first, order->m_isBuy) &&
             qtyToMatch > 0;) {
            auto bestPrice = itBestPrice->first;
            PriceNode *bestPriceNode = itBestPrice->second;
            OrderTree *quotes = bestPriceNode->getOrderTree();

            // for each order (in size > time sequence) in this price level
            match(quotes->begin(), order, qtyToMatch, quotes, bestPrice);

            // order depletes current price level
            if (quotes->empty()) {
                delete bestPriceNode;
                // bestPriceNode = NULL;
                m_askTree->erase(itBestPrice++);
                m_askMap->erase(bestPrice);
            } else
                ++itBestPrice;
        }
    } else {
        for (PriceTreeRevIt itBestPrice = m_bidTree->rbegin();
             itBestPrice != m_bidTree->rend() && isMarketable(order, m_bidTree->rbegin()->first, order->m_isBuy) &&
             qtyToMatch > 0;) {
            auto bestPrice = itBestPrice->first;
            PriceNode *bestPriceNode = itBestPrice->second;
            OrderTree *quotes = bestPriceNode->getOrderTree();

            // for each order (in size > time sequence) in this price level
            match(quotes->begin(), order, qtyToMatch, quotes, bestPrice);

            // order depletes current price level
            if (quotes->empty()) {
                delete bestPriceNode;
                // bestPriceNode = NULL;
                m_bidTree->erase(std::next(itBestPrice).base());  // erase in reverse iterator
                m_bidMap->erase(bestPrice);
            } else
                ++itBestPrice;
        }
    }
}

inline void OrderBook::match(OrderTreeIt it, const Order *order, Volume &qtyToMatch, OrderTree *quotes,
                             Price bestPrice) {
    bool isBuy = order->m_isBuy;

    while (it != quotes->end() && order != NULL && isMarketable(order, bestPrice, isBuy) && qtyToMatch > 0) {
        Order *quote = *it;
        quotes->erase(it++);

        auto curQty = quote->m_quantity;
        auto execQty = min(curQty, qtyToMatch);
        const string &buyer = isBuy ? order->m_name : quote->m_name;
        const string &seller = isBuy ? quote->m_name : order->m_name;
        bookTrade(execQty, buyer, seller);
        qtyToMatch -= execQty;

        // add residual back to order tree
        if (curQty > execQty) {
            quote->m_quantity = curQty - execQty;
            quotes->emplace(quote);
        } else
            delete quote;
    }
    // order depletes current quote
    if (qtyToMatch == 0) delete order;
}

/**
 * NonMarketable order handling:
 *  Add liquidity to the same side of the book given and order
 *  Assume M is the avg number of quotes in the order book
 * */
inline void OrderBook::add(Order *order) {
    bool isBuy = order->m_isBuy;
    auto price = order->m_price;
    PriceTree *priceTree = isBuy ? m_bidTree : m_askTree;
    PriceToNodeMap *priceToNodeMap = isBuy ? m_bidMap : m_askMap;

    PriceToNodeMapIt it = priceToNodeMap->find(price);
    if (it != priceToNodeMap->end()) {
        OrderTree *quotes = it->second->getOrderTree();
        quotes->emplace(order);
    } else {
        PriceNode *priceNode = new PriceNode(price);
        priceNode->insertOrder(order);
        priceTree->emplace(price, priceNode);
        priceToNodeMap->emplace(price, priceNode);
    }
}

inline void OrderBook::bookTrade(Volume qty, const string &buyer, const string &seller) {
    AccountMapIt it = m_account->find(buyer);
    if (it != m_account->end()) it->second += qty;

    it = m_account->find(seller);
    if (it != m_account->end()) it->second -= qty;
}

inline void OrderBook::bookTradeForInstrument(const vector<string> &names) {
    for (std::string name : names) m_account->emplace(name, 0);
}

inline int OrderBook::getTraderExposure(const string &name) {
    AccountMapIt it = m_account->find(name);
    if (it != m_account->end())
        return it->second;
    else
        return 0;
}

}  // namespace fun::Matching
