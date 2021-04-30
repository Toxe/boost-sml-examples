#pragma once

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

struct my_logger {
    template <class SM, class TEvent>
    void log_process_event(const TEvent&)
    {
        spdlog::info("({}) process_event: {}", boost::sml::aux::get_type_name<SM>(), boost::sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TGuard, class TEvent>
    void log_guard(const TGuard&, const TEvent&, bool result)
    {
        spdlog::info("({})         guard: {} {} {}", boost::sml::aux::get_type_name<SM>(), boost::sml::aux::get_type_name<TGuard>(),
            boost::sml::aux::get_type_name<TEvent>(), (result ? "[OK]" : "[Reject]"));
    }

    template <class SM, class TAction, class TEvent>
    void log_action(const TAction&, const TEvent&)
    {
        spdlog::info("({})        action: {} {}", boost::sml::aux::get_type_name<SM>(), boost::sml::aux::get_type_name<TAction>(),
            boost::sml::aux::get_type_name<TEvent>());
    }

    template <class SM, class TSrcState, class TDstState>
    void log_state_change(const TSrcState& src, const TDstState& dst)
    {
        spdlog::info("({})    transition: {} -> {}", boost::sml::aux::get_type_name<SM>(), src.c_str(), dst.c_str());
    }
};
