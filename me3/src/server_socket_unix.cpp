#include "server_socket_unix.hpp"

#include <iostream>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>

server_socket_unix::server_socket_unix(const char* sock_name)
    : sock_name_{sock_name}
{
    fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_ == -1)
    {
        std::cout << "socket error" << std::endl;
        return;
    }

    int yes = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        std::cout << "SO_REUSEADDR error" << std::endl;
        return;
    }

    sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strncpy(sock_addr.sun_path, sock_name_, sizeof(sock_addr.sun_path)-1);

    if (bind(fd_, (const sockaddr*)&sock_addr, sizeof(sockaddr_un)) == -1)
    {
        std::cout << "bind error" << std::endl;
        return;
    }

    if (listen(fd_, BACKLOG) == -1)
    {
        std::cout << "listen error" << std::endl;
        return;
    }
}

void server_socket_unix::accept_conn()
{
    client_fd_ = accept(fd_, nullptr, nullptr);
    if (client_fd_ == -1)
    {
        std::cout << "accept error" << std::endl;
    }
}

char* server_socket_unix::recv_msg()
{
    if (recv(client_fd_, buf_, BUF_SIZE, 0) <= 0)
    {
        std::cout << "server_socket_unix recv_msg error" << std::endl;
        throw "server_socket_unix recv_msg error";
    }
    return buf_;
}

void server_socket_unix::send_msg(const char* buf, const int size) const
{
    send(client_fd_, buf, size, MSG_DONTWAIT);
}

server_socket_unix::~server_socket_unix()
{
    close(fd_);
    close(client_fd_);
    unlink(sock_name_);
}
