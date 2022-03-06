/*
 * Copyright (C) 2022
 */

#pragma once

#include "feed_player.hpp"

namespace fun::client {

class FeedPlayerRecorder : public FeedPlayer {
protected:
    std::unordered_map<Xoid, Coid> orderReverseMapping_;

public:
    FeedPlayerRecorder();
    void play(Client &client, std::list<FeedMessage> feedMessages, const std::string &recordedFile);
};

}  // namespace fun::client
