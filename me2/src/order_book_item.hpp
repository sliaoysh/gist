#pragma once

#include "types.hpp"

struct order_book_item
{
    fun::client::Xoid ex_order_id;
    fun::client::Volume volume;
};
