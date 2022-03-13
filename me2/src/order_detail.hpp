#pragma once

#include "types.hpp"

struct order_detail
{
    fun::client::Price price;
    fun::client::Volume volume;
    fun::client::InstrumentId instrument_id;
    fun::client::Side side;
};
