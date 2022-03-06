/*
 * Copyright (C) 2022
 */

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <iostream>

#include "feed_player.hpp"
#include "matching_engine.hpp"

using namespace std;

using namespace std;
using namespace fun::client;

struct CmdOption {
    string feedFile;
    string meAddress;
    int mePort;
};

int start(const CmdOption &option) {
    MatchingEngine me = MatchingEngine(option.meAddress, option.mePort);
    me.run(option.feedFile);

    return 0;
}

int main(int argc, char *argv[]) {
    CLI::App app{"Matching Engine"};

    static const string feedFile = "../data/feed.csv";
    CmdOption option{feedFile, "127.0.0.1", 8001};
    app.add_option("-p,--port", option.mePort, "The ME port");
    app.add_option("-a,--me-address", option.meAddress, "The ME Address");
    app.add_option("-f,--feed-file", option.feedFile, "The feed csv file");

    CLI11_PARSE(app, argc, argv);
    return start(option);
}
