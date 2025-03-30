#include <liburing.h>
#include <array>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <string.h>
#include <string_view>
#include <vector>
using namespace std;

// g++ -Wall -Wextra -Wpedantic -std=c++20 -m64 -O3 -flto -march=native -luring -o cat cat.cpp -fsanitize=address,undefined -lasan

/*
struct iovec {
    void   *iov_base;  // Starting address
    size_t  iov_len;   // Size of the memory pointed to by iov_base
};
*/

class cat
{
public:
    cat(filesystem::path p)
    {
        const int file_size = filesystem::file_size(p);
        const int tail_size = file_size % BLK_SIZE;
        const int blocks = file_size / BLK_SIZE + (tail_size > 0);
        const int fd = open(p.c_str(), O_RDONLY);
        if (fd < 0){ throw strerror(errno); }
        v.resize(blocks);
        buf.reserve(blocks);
        for (int i = 0; i < blocks; ++i)
        {
            buf.push_back(iovec{v[i].data(),BLK_SIZE});
        }
        if (tail_size){ buf.back().iov_len = tail_size; }

        io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
        io_uring_sqe* sqe = io_uring_get_sqe(&ring);
        io_uring_prep_readv(sqe, fd, buf.data(), blocks, 0);
        io_uring_sqe_set_data(sqe, reinterpret_cast<void*>(fd));
        io_uring_submit(&ring);
    }

    ~cat()
    {
        io_uring_queue_exit(&ring);
    }

    template <typename F>
    void get(F&& cb)
    {
        io_uring_cqe *cqe;
        const int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0){ throw strerror(errno); }
        if (cqe->res < 0)
        {
            cout << cqe->res << endl;
            cout << strerror(-cqe->res) << endl;
            throw "Async readv failed";
        }
        const int fd = reinterpret_cast<long long>(io_uring_cqe_get_data(cqe));
        for (const auto& e : buf)
        {
            cb(string_view{static_cast<char*>(e.iov_base), e.iov_len});
        }
        io_uring_cqe_seen(&ring, cqe);
        close(fd);
    }

private:
    static constexpr int QUEUE_DEPTH = 1;
    static constexpr int BLK_SIZE = 1024;
    vector<iovec> buf;
    vector<array<char,BLK_SIZE>> v;
    io_uring ring;
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: cat file_name" << endl;
        return 0;
    }

    cat c1{argv[1]};
    c1.get(
        [](string_view str)
        {
            cout << str;
        }
    );

    cout << "\n*** end ***" << endl;
}
