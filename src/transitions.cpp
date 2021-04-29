#include <cassert>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct state1 {};
struct state2 {};
struct state3 {};
struct state4 {};

struct event1 {};
struct event2_internal {};
struct event2_leave {};
struct event4 {};

auto guard = [] { return true; };

auto action1 = [] {};
auto action2 = [] { spdlog::info("internal transition"); };
auto action3 = [] {};
auto action4 = [] { spdlog::info("anonymous transition"); };
auto action5 = [] {};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [ guard ] / action1 = state<state2>,
            state<state2> + event<event2_internal>  / action2,                  // internal transition
            state<state2> + event<event2_leave>     / action3 = state<state3>,
            state<state3>                           / action4 = state<state4>,  // anonymous transition
            state<state4> + event<event4> [ guard ] / action5 = X
        );
    }
};

int main()
{
    sml::sm<state_machine> sm;

    sm.process_event(event1{});
    assert(sm.is(sml::state<state2>));
    sm.process_event(event2_internal{});
    assert(sm.is(sml::state<state2>));
    sm.process_event(event2_leave{});  // immediate anonymous transition from state3 to state4
    assert(sm.is(sml::state<state4>));
    sm.process_event(event4{});

    assert(sm.is(sml::X));
}
