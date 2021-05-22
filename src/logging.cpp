#include <cassert>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

#include "common/dump.h"
#include "common/logger.h"

namespace sml = boost::sml;

// states
struct state1 {};
struct state2 {};
struct state3 {};
struct state4 {};
struct state5 {};

// events
struct event1 {};
struct event2 { int value_; };
struct event3 {};
struct event4 { int value_; };

// guards
auto guard1 = [] { return true; };
auto guard2 = [] { return true; };
auto guard3 = [] { return true; };

struct guard4 {
    bool operator()() {
        return true;
    }
};

// actions
auto action1 = [] {
    #if defined(_MSC_VER) && !defined(__clang__)
    spdlog::info("action1. __FUNCTION__: {}, __FUNCSIG__: {}", __FUNCTION__, __FUNCSIG__);
    #elif defined(__clang__) || defined(__GNUC__)
    spdlog::info("action1. __PRETTY_FUNCTION__: {}", __PRETTY_FUNCTION__);
    #endif
};

auto action2 = [](const event2& event) {
    #if defined(_MSC_VER) && !defined(__clang__)
    spdlog::info("action2, value={}. __FUNCTION__: {}, __FUNCSIG__: {}", event.value_, __FUNCTION__, __FUNCSIG__);
    #elif defined(__clang__) || defined(__GNUC__)
    spdlog::info("action2, value={}. __PRETTY_FUNCTION__: {}", event.value_, __PRETTY_FUNCTION__);
    #endif
};

struct action3 {
    void operator()() {
        #if defined(_MSC_VER) && !defined(__clang__)
        spdlog::info("action3. __FUNCTION__: {}, __FUNCSIG__: {}", __FUNCTION__, __FUNCSIG__);
        #elif defined(__clang__) || defined(__GNUC__)
        spdlog::info("action3. __PRETTY_FUNCTION__: {}", __PRETTY_FUNCTION__);
        #endif
    }
};

struct action4 {
    void operator()(const event4& event) {
        #if defined(_MSC_VER) && !defined(__clang__)
        spdlog::info("action4, value={}. __FUNCTION__: {}, __FUNCSIG__: {}", event.value_, __FUNCTION__, __FUNCSIG__);
        #elif defined(__clang__) || defined(__GNUC__)
        spdlog::info("action4, value={}. __PRETTY_FUNCTION__: {}", event.value_, __PRETTY_FUNCTION__);
        #endif
    }
};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<state1> + event<event1> [ guard1   ] / action1   = state<state2>,
            state<state2> + event<event2> [ guard2   ] / action2   = state<state3>,
            state<state3> + event<event3> [ guard3   ] / action3{} = state<state4>,
            state<state4> + event<event4> [ guard4{} ] / action4{} = state<state5>,
            state<state5> = X
        );
    }
};

int main()
{
    my_logger logger;
    sml::sm<state_machine, sml::logger<my_logger>> sm{logger};

    dump(sm);

    sm.process_event(event1{});
    sm.process_event(event2{10});
    sm.process_event(event3{});
    sm.process_event(event4{20});

    assert(sm.is(sml::X));
}
