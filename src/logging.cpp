#include <cassert>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct my_logger {
    template <class SM, class TEvent>
    void log_process_event(const TEvent&)
    {
        spdlog::info("({}) process_event: {}", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TGuard, class TEvent>
    void log_guard(const TGuard&, const TEvent&, bool result)
    {
        spdlog::info("({})         guard: {} {} {}", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TGuard>(),
            sml::aux::get_type_name<TEvent>(), (result ? "[OK]" : "[Reject]"));
    }

    template <class SM, class TAction, class TEvent>
    void log_action(const TAction&, const TEvent&)
    {
        spdlog::info("({})        action: {} {}", sml::aux::get_type_name<SM>(), sml::aux::get_type_name<TAction>(),
            sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState& src, const TDstState& dst)
    {
        spdlog::info("({})    transition: {} -> {}", sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str());
    }
};

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

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [ guard1 ] / action1 = state<state2>,
            state<state2> + event<event2> [ guard2 ] / action2 = state<state3>,
            state<state3> + event<event3> [ guard3 ] / action3 = X
        );
    }
};

int main()
{
    my_logger logger;
    sml::sm<state_machine, sml::logger<my_logger>> sm{logger};

    sm.process_event(event1{});
    sm.process_event(event2{});
    sm.process_event(event3{});

    assert(sm.is(sml::X));
}
