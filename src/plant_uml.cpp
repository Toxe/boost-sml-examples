#include <cassert>
#include <iostream>
#include <string>

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

template <class T>
void dump_transition() noexcept
{
    auto src_state = std::string{sml::aux::string<typename T::src_state>{}.c_str()};
    auto dst_state = std::string{sml::aux::string<typename T::dst_state>{}.c_str()};
    if (dst_state == "X") {
        dst_state = "[*]";
    }

    if (T::initial) {
        std::cout << "[*] --> " << src_state << std::endl;
    }

    const auto has_event = !sml::aux::is_same<typename T::event, sml::anonymous>::value;
    const auto has_guard = !sml::aux::is_same<typename T::guard, sml::front::always>::value;
    const auto has_action = !sml::aux::is_same<typename T::action, sml::front::none>::value;

    const auto is_entry = sml::aux::is_same<typename T::event, sml::back::on_entry<sml::_, sml::_>>::value;
    const auto is_exit = sml::aux::is_same<typename T::event, sml::back::on_exit<sml::_, sml::_>>::value;

    // entry / exit entry
    if (is_entry || is_exit) {
        std::cout << src_state;
    } else { // state to state transition
        std::cout << src_state << " --> " << dst_state;
    }

    if (has_event || has_guard || has_action) {
        std::cout << " :";
    }

    if (has_event) {
        // handle 'on_entry' and 'on_exit' per plant-uml syntax
        auto event = std::string(boost::sml::aux::get_type_name<typename T::event>());
        if (is_entry) {
            event = "entry";
        } else if (is_exit) {
            event = "exit";
        }
        std::cout << " " << event;
    }

    if (has_guard) {
        std::cout << " [" << boost::sml::aux::get_type_name<typename T::guard::type>() << "]";
    }

    if (has_action) {
        std::cout << " / " << boost::sml::aux::get_type_name<typename T::action::type>();
    }

    std::cout << std::endl;
}

template <template <class...> class T, class... Ts>
void dump_transitions(const T<Ts...>&) noexcept
{
    int _[]{0, (dump_transition<Ts>(), 0)...};
    (void) _;
}

template <class SM>
void dump(const SM&) noexcept
{
    std::cout << "@startuml" << std::endl
              << std::endl;
    dump_transitions(typename SM::transitions{});
    std::cout << std::endl
              << "@enduml" << std::endl;
}

int main()
{
    sml::sm<state_machine> sm;
    dump(sm);
}
