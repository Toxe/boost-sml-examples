#include <cassert>
#include <functional>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

#include "common/logger.h"

namespace sml = boost::sml;

struct state1 {};
struct state2 {};
struct state3 {};
struct state4 {};

struct event1 {
    int count_;
};

struct event2 {};
struct event3 {};
struct event4 {};

auto guard1 = [](const event1& e) { return e.count_ == 0; };

struct guard2 {
    bool operator()() { return true; }
};

bool guard3() {
    return true;
}

std::function<bool(void)> guard3_fnc = guard3;

auto action1a = [] { spdlog::info("leave state1"); };
auto action1b = [] { spdlog::info("stay in state1"); };
auto action2  = [] { spdlog::info("leave state2"); };
auto action3  = [] { spdlog::info("leave state3"); };
auto action4  = [] { spdlog::info("leave state4"); };

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [  guard1     ] / action1a = state<state2>,
            state<state1> + event<event1> [ !guard1     ] / action1b,
            state<state2> + event<event2> [  guard2{}   ] / action2 = state<state3>,
            state<state3> + event<event3> [  guard3_fnc ] / action3 = state<state4>,
            state<state4> + event<event4>                 / action4 = X
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
    sm.process_event(event4{});

    assert(sm.is(sml::X));
}
