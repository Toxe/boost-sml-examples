#include <cassert>
#include <typeinfo>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct state1 {};
struct state2 {};
struct state3 {};
struct state4 {};
struct state5 {};

struct event1 {};
struct event2 {};
struct event3 {};
struct event4 {};
struct event5 {};

auto action1 = [] { spdlog::info("simple action1"); };
auto action2 = [](event2 e) { spdlog::info("action2 called with event of type \"{}\"", typeid(e).name()); };
auto action3a = [] {};
auto action3b = [] {};

struct action4 {
    void operator()() {
        spdlog::info("called action4");
    }
};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> / action1              = state<state2>,
            state<state2> + event<event2> / action2              = state<state3>,
            state<state3> + event<event3> / (action3a, action3b) = state<state4>,
            state<state4> + event<event4> / action4{}            = state<state5>,
            state<state5> + event<event5>                        = X
        );
    }
};

int main()
{
    sml::sm<state_machine> sm;

    sm.process_event(event1{});
    sm.process_event(event2{});
    sm.process_event(event3{});
    sm.process_event(event4{});
    sm.process_event(event5{});

    assert(sm.is(sml::X));
}
