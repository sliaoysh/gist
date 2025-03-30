#include <liburing.h>
#include <array>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string_view>
#include <vector>
using namespace std;

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
        fd = open(p.c_str(), O_RDONLY);
        if (fd < 0){ throw strerror(errno); }
        v.resize(blocks);
        buf.reserve(blocks);
        for (int i = 0; i < blocks; ++i)
        {
            buf.push_back(iovec{v[i].data(),BLK_SIZE});
        }
        if (tail_size){ buf.back().iov_len = tail_size; }
        int res = readv(fd, buf.data(), blocks);
        if (res < 0)
        {
            const int e = errno;
            cout << strerror(e) << endl;
            cout << e << endl;
            cout << "res: " << res << endl;
        }
    }

    ~cat()
    {
        close(fd);
    }

    template <typename F>
    void get(F&& cb)
    {
        for (const auto& e : buf)
        {
            cb(string_view{static_cast<char*>(e.iov_base), e.iov_len});
        }
    }

private:
    static constexpr int QUEUE_DEPTH = 1;
    static constexpr int BLK_SIZE = 1024;
    vector<iovec> buf;
    vector<array<char,BLK_SIZE>> v;
    io_uring ring;
    int fd;
};

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cout << "Usage: readv file_name" << endl;
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
