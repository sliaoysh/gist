#include <liburing.h>
#include <iostream>
using namespace std;

int main()
{
    io_uring ring;
    io_uring_queue_init(1, &ring, 0);
    io_uring_queue_exit(&ring);
    cout << "ok" << endl;
}
