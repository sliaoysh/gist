#pragma once

class server_socket
{
public:
    static constexpr int BACKLOG = 1;
    static constexpr int BUF_SIZE = 256;

    server_socket(const char* ip, int port);
    ~server_socket();
    void accept_conn();
    char* recv_msg();
    void send_msg(const char* buf, int size) const;

private:
    int fd_;
    int client_fd_;
    char buf_[BUF_SIZE];
};
