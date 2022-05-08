#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
using namespace std;

struct event_connect { string_view addr; };
struct event_connected {};
struct event_disconnect {};
struct event_timeout {};
using event =
      variant<event_connect, event_connected, event_disconnect, event_timeout>;

struct state_idle {};
struct state_connecting
{
  static constexpr int max = 3;
  string addr;
  int n = 0;
};
struct state_connected {};
using state = variant<state_idle, state_connecting, state_connected>;

struct transitions
{
  optional<state> operator()(const state_idle&, const event_connect& e) const
  {
    cout << "state_idle -> state_connecting" << endl;
    return state_connecting{string{e.addr}};
  }

  optional<state> operator()(const state_connecting&, const event_connected&) const
  {
    cout << "state_connecting -> state_connected" << endl;
    return state_connected{};
  }

  optional<state> operator()(state_connecting& s, const event_timeout&) const
  {
    if(++s.n < state_connecting::max)
    {
      cout << "state unchange" << endl;
      return nullopt;
    }
    cout << "state_connecting -> state_idle" << endl;
    return state_idle{};
  }

  optional<state> operator()(const state_connected&, const event_disconnect&) const
  {
    cout << "state_connected -> state_idle" << endl;
    return state_idle{};
  }

  template<typename State, typename Event>
  optional<state> operator()(const State&, const Event&) const
  {
    cout << "state unchange" << endl;
    return nullopt;
  }
};

template<typename StateVariant, typename EventVariant, typename Transitions>
class fsm
{
  StateVariant state_;
public:
  void dispatch(const EventVariant& event)
  {
    auto new_state = visit(Transitions{}, state_, event);
    if(new_state)
    {
      state_ = move(*new_state);
    }
  }
};

using connection_fsm = fsm<state, event, transitions>;

int main()
{
  connection_fsm conn_fsm;
  conn_fsm.dispatch(event_connect{"xxx"});
  conn_fsm.dispatch(event_timeout{});
  conn_fsm.dispatch(event_timeout{});
  conn_fsm.dispatch(event_timeout{});
  conn_fsm.dispatch(event_connect{"yyy"});
  conn_fsm.dispatch(event_connected{});
  conn_fsm.dispatch(event_disconnect{});
  conn_fsm.dispatch(event_disconnect{});
  return 0;
}
