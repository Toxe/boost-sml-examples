#pragma once

#include <iostream>
#include <string>

#include <boost/sml.hpp>

template <class T>
void dump_transition() noexcept
{
    auto src_state = std::string{boost::sml::aux::string<typename T::src_state>{}.c_str()};
    auto dst_state = std::string{boost::sml::aux::string<typename T::dst_state>{}.c_str()};
    if (dst_state == "X") {
        dst_state = "[*]";
    }

    if constexpr (T::initial) {
        std::cout << "[*] --> " << src_state << std::endl;
    }

    const auto has_event = !boost::sml::aux::is_same<typename T::event, boost::sml::anonymous>::value;
    const auto has_guard = !boost::sml::aux::is_same<typename T::guard, boost::sml::front::always>::value;
    const auto has_action = !boost::sml::aux::is_same<typename T::action, boost::sml::front::none>::value;

    const auto is_entry = boost::sml::aux::is_same<typename T::event, boost::sml::back::on_entry<boost::sml::_, boost::sml::_>>::value;
    const auto is_exit = boost::sml::aux::is_same<typename T::event, boost::sml::back::on_exit<boost::sml::_, boost::sml::_>>::value;

    // entry / exit entry
    if constexpr (is_entry || is_exit) {
        std::cout << src_state;
    } else { // state to state transition
        std::cout << src_state << " --> " << dst_state;
    }

    if constexpr (has_event || has_guard || has_action) {
        std::cout << " :";
    }

    if constexpr (has_event) {
        // handle 'on_entry' and 'on_exit' per plant-uml syntax
        auto event = std::string(boost::sml::aux::get_type_name<typename T::event>());
        if (is_entry) {
            event = "entry";
        } else if (is_exit) {
            event = "exit";
        }
        std::cout << " " << event;
    }

    if constexpr (has_guard) {
        std::cout << " [" << boost::sml::aux::get_type_name<typename T::guard::type>() << "]";
    }

    if constexpr (has_action) {
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
