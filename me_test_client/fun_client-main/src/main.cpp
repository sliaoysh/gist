/*
 * Copyright (C) 2022
 */

#include <CLI/App.hpp>
#include <CLI/Config.hpp>
#include <CLI/Formatter.hpp>
#include <iostream>

#include "logger.hpp"
#include "feed_player.hpp"
#include "feed_player_recorder.hpp"
#include "fun_client.hpp"
#include "unix_socket.hpp"

using namespace std;

using namespace std;
using namespace fun::client;

static Logger logger;

struct CmdOption {
    string feedFile;
    string meAddress;
    int mePort;
    string recordFile;
    string logLevel;
    string unixSocketFile;
};

int runPlayer(const CmdOption &option, Client &client) {
    if (option.recordFile.empty()) {
        FeedPlayer player;
        auto feedMessages = player.process_csv(option.feedFile);
        player.play(client, feedMessages);
        player.dumpTiming();
    } else {
        FeedPlayerRecorder player;
        auto feedMessages = player.process_csv(option.feedFile);
        player.play(client, feedMessages, option.recordFile);
        player.dumpTiming();
    }
    return 0;
}

int start(const CmdOption &option) {
    if (option.meAddress.empty()) {
        UnixSocket sock;
        sock.connect(option.unixSocketFile);
        Client client(sock.getReceiving(), sock.getSending());
        return runPlayer(option, client);
    } else {
        TcpSocket sock;
        sock.connect(option.meAddress, option.mePort);
        Client client(sock.getReceiving(), sock.getSending());
        return runPlayer(option, client);
    }
}

int main(int argc, char *argv[]) {
    CLI::App app{"ME client"};

    static const string feedFile = "../data/feed.csv";
    CmdOption option{feedFile, "", 8001, "", "info"};
    app.add_option("-p,--port", option.mePort, "The ME port");
    app.add_option("-a,--me-address", option.meAddress, "The ME Address");
    app.add_option("-s,--socket-file", option.unixSocketFile, "The Unix socket file");
    app.add_option("-f,--feed-file", option.feedFile, "The feed csv file");
    app.add_option("-r,--record-file", option.recordFile, "Record file to output");
    app.add_option("-l,--log-level", option.logLevel, "Log level: [error, info, debug, trace]");

    CLI11_PARSE(app, argc, argv);
    auto logLevel = spdlog::level::debug;
    if (option.logLevel == "debug") {
        logLevel = spdlog::level::debug;
    } else if (option.logLevel == "info") {
        logLevel = spdlog::level::info;
    } else if (option.logLevel == "trace") {
        logLevel = spdlog::level::trace;
    } else if (option.logLevel == "error") {
        logLevel = spdlog::level::err;
    }
    spdlog::set_level(logLevel);
    return start(option);
}
