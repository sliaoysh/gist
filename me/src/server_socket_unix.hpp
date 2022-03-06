#pragma once

class server_socket_unix
{
public:
    static constexpr int BACKLOG = 1;
    static constexpr int BUF_SIZE = 256;

    server_socket_unix(const char* sock_name);
    ~server_socket_unix();
    void accept_conn();
    char* recv_msg();
    void send_msg(const char* buf, int size) const;

private:
    const char* sock_name_;
    int fd_;
    int client_fd_;
    char buf_[BUF_SIZE];
};
