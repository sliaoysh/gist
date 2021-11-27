// g++ -std=c++20 -m64 -O3 -Wall -Wextra -Wpedantic -o ev2 ev2.cpp -lev

#include <ev.h>
#include <iostream>
using namespace std;

// this callback is called when data is readable on stdin
void stdin_cb(EV_P_ ev_io* w, int revents)
{
    cout << "stdin_cb" << endl;
    cout << "EV_READ: " << (revents & EV_READ) << endl;

    string s;
    cin >> s;
    if (s == "q")
    {
        ev_io_stop (EV_A_ w);
        ev_break (EV_A_ EVBREAK_ALL);
    }
    else
    {
        cout << s << "\n" << endl;
    }
}

void timeout_cb(EV_P_ ev_timer*, int revents)
{
    cout << "timeout_cb" << endl;
    cout << "EV_READ: " << (revents & EV_READ) << endl;
    cout << "EV_TIMER: " << (revents & EV_TIMER) << '\n' << endl;

    static int n = 10;
    if (!--n)
    {
        ev_break (EV_A_ EVBREAK_ONE);
    }
}

int main()
{
    ev_io stdin_watcher;
    ev_timer timeout_watcher;
    struct ev_loop *loop = EV_DEFAULT;

    ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/0, EV_READ);
    ev_io_start(loop, &stdin_watcher);

    ev_timer_init (&timeout_watcher, timeout_cb, 5, 4);
    ev_timer_start (loop, &timeout_watcher);

    ev_run (loop, 0);
    cout << "end" << endl;
    return 0;
}
