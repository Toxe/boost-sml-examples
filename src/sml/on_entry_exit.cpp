#include <cassert>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct state1 {};
struct state2 {};
struct state3 {};

struct event1 {};
struct event2 {};
struct event3 {};

auto guard1 = [] { return true; };
auto guard2 = [] { return true; };
auto guard3 = [] { return true; };

auto action1 = [] {};
auto action2 = [] {};
auto action3 = [] {};

struct simple {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [ guard1 ] / action1 = state<state2>,
            state<state2> + event<event2> [ guard2 ] / action2 = state<state3>,
            state<state3> + event<event3> [ guard3 ] / action3 = X,
            state<state1> + on_entry<_> / [] { spdlog::info("state1: on_entry"); },
            state<state1> + on_exit<_>  / [] { spdlog::info("state1: on_exit"); },
            state<state2> + on_entry<_> / [] { spdlog::info("state2: on_entry"); },
            state<state2> + on_exit<_>  / [] { spdlog::info("state2: on_exit"); },
            state<state3> + on_entry<_> / [] { spdlog::info("state3: on_entry"); },
            state<state3> + on_exit<_>  / [] { spdlog::info("state3: on_exit"); }
        );
    }
};

int main()
{
    sml::sm<simple> sm;

    sm.process_event(event1{});
    sm.process_event(event2{});
    sm.process_event(event3{});

    assert(sm.is(sml::X));
}
