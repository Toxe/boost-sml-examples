#include <cassert>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct state1 {};
struct state2 {};
struct state3 {};

struct event1 {
    int count_;
    event1(const int count) : count_{count} {}
};

struct event2 {};
struct event3 {};

auto guard1 = [](const event1& e) { return e.count_ == 0; };
auto guard2 = [] { return true; };

auto action1 = [] {};
auto action2 = [] {};
auto action3 = [] {};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [ guard1 ] / action1 = state<state2>,
            state<state2> + event<event2> [ guard2 ] / action2 = state<state3>,
            state<state3> + event<event3>            / action3 = X
        );
    }
};

int main()
{
    sml::sm<state_machine> sm;

    sm.process_event(event1{2});
    assert(sm.is(sml::state<state1>));  // stay in state1
    sm.process_event(event1{1});
    assert(sm.is(sml::state<state1>));  // stay in state1
    sm.process_event(event1{0});
    assert(sm.is(sml::state<state2>));  // --> transition to state2

    sm.process_event(event2{});
    sm.process_event(event3{});

    assert(sm.is(sml::X));
}
