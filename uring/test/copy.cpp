#include <liburing.h>
#include <array>
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string.h>
#include <string_view>
#include <vector>
using namespace std;

// g++ -Wall -Wextra -Wpedantic -std=c++20 -m64 -O3 -flto -march=native -luring -o copy copy.cpp -fsanitize=address,undefined -lasan

/*
struct iovec {
    void   *iov_base;  // Starting address
    size_t  iov_len;   // Size of the memory pointed to by iov_base
};
*/

static constexpr int BLK_SIZE = 4096;

struct job
{
    iovec task;
    int offset;
    int init_offset;
    int init_len;
    bool is_read;
    array<uint8_t,BLK_SIZE> mem;
};

void submit(io_uring& ring)
{
    const int ret = io_uring_submit(&ring);
    if (ret < 0)
    {
        throw strerror(-ret);
    }
}

void prep(io_uring& ring, job& data, const int fd)
{
    io_uring_sqe* sqe = io_uring_get_sqe(&ring);

    if (data.is_read)
        io_uring_prep_readv(sqe, fd, &data.task, 1, data.offset);
    else
        io_uring_prep_writev(sqe, fd, &data.task, 1, data.offset);

    io_uring_sqe_set_data(sqe, &data);
}

void read(io_uring& ring, job& data, const int size, const int offset, const int fd)
{
    data.task.iov_base = data.mem.data();
    data.task.iov_len = size;
    data.init_len = size;
    data.offset = offset;
    data.init_offset = offset;
    data.is_read = true;
    prep(ring, data, fd);
}

void write(io_uring& ring, job& data, const int fd)
{
    data.is_read = false;
    data.offset = data.init_offset;
    data.task.iov_base = data.mem.data();
    data.task.iov_len = data.init_len;
    prep(ring, data, fd);
}

void copy(filesystem::path src_path, filesystem::path dst_path)
{
    const int src = open(src_path.c_str(), O_RDONLY);
    if (src < 0){ throw strerror(errno); }
    const int dst = open(dst_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst < 0){ throw strerror(errno); }

    static constexpr int QD = 2;
    const int file_size = filesystem::file_size(src_path);
    io_uring ring;

    int ret = io_uring_queue_init(QD, &ring, 0);
    if(ret < 0){ throw strerror(-ret); }

    vector<int> free_slots(QD);
    iota(begin(free_slots), end(free_slots), 0);
    array<job,QD> jobs;
    int outstanding = file_size;

    while (outstanding || free_slots.size() != QD)
    {
        if (outstanding && !free_slots.empty())
        {
            const int i = free_slots.back();
            free_slots.pop_back();
            const int size = min(BLK_SIZE, outstanding);
            read(ring, jobs[i], size, file_size-outstanding, src);
            outstanding -= size;

            if (free_slots.empty() || !outstanding)
            {
                submit(ring);
            }
        }
        else
        {
            io_uring_cqe* cqe;
            ret = io_uring_wait_cqe(&ring, &cqe);
            if (ret < 0){ throw strerror(-ret); }
            job* data = reinterpret_cast<job*>(io_uring_cqe_get_data(cqe));

            if (cqe->res < 0)
            {
                if (cqe->res == -EAGAIN)
                {
                    prep(ring, *data, (data->is_read? src : dst));
                }
                else{ throw strerror(-cqe->res); }
            }
            else if ((size_t)cqe->res != data->task.iov_len)
            {
                // short read/write, adjust and requeue
                data->task.iov_base = (char*)data->task.iov_base + cqe->res;
                data->task.iov_len -= cqe->res;
                data->offset += cqe->res;
                prep(ring, *data, (data->is_read? src : dst));
            }
            else
            {
                if (data->is_read)
                {
                    write(ring, *data, dst);
                }
                else
                {
                    const int i = distance(jobs.data(), data);
                    free_slots.push_back(i);
                }
            }
            io_uring_cqe_seen(&ring, cqe);
            submit(ring);
        }
    }

    close(src);
    close(dst);
    io_uring_queue_exit(&ring);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout << "Usage: copy src dst" << endl;
        return 0;
    }
    try
    {
        copy(argv[1], argv[2]);
    }
    catch(const char* e)
    {
        cout << "error:\n";
        cout << e << endl;
    }
    cout << "done" << endl;
}
