#include <cassert>
#include <iostream>
#include <string>

#include <boost/sml.hpp>
#include <spdlog/spdlog.h>

#include "common/dump.h"
#include "common/logger.h"

namespace sml = boost::sml;

struct supervisor {
    int waiting_for_results_ = 0;

    void set(int n) {
        spdlog::info("[supervisor] setting waiting_for_results_ {} --> {}", waiting_for_results_, n);
        waiting_for_results_ = n;
    }

    void dec() {
        spdlog::info("[supervisor] decrease waiting_for_results_ {} --> {}", waiting_for_results_, waiting_for_results_ - 1);
        --waiting_for_results_;
    }
};

// states
struct starting {};
struct idling {};
struct calculating {};
struct receiving_calculation_results {};
struct receiving_colorization_results {};
struct coloring {};
struct canceling_calculation {};
struct canceling_colorization {};
struct shutting_down {};

// events
struct started {};
struct image_request { int n_; };
struct calculation_results {};
struct colorization_results {};
struct quit {};
struct cancel {};

// actions
struct send_calculation_messages {
    void operator()(supervisor& sv, const image_request& e) {
        spdlog::info("[action] send_calculation_messages, n={}", e.n_);
        sv.set(e.n_);
    }
};

struct send_colorization_messages {
    void operator()(supervisor& sv) {
        sv.set(4);
    }
};

struct receive_calculation_results {
    void operator()(supervisor& sv) {
        sv.dec();
    }
};

struct receive_colorization_results {
    void operator()(supervisor& sv) {
        sv.dec();
    }
};

// guards
struct has_all_results {
    bool operator()(const supervisor& sv) {
        return sv.waiting_for_results_ == 0;
    }
};

struct has_not_all_results {
    bool operator()(const supervisor& sv) {
        return sv.waiting_for_results_ > 0;
    }
};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        // clang-format off
        return make_transition_table(
          * state<starting>               + event<started>                                                                         = state<idling>,
            state<idling>                 + event<quit>                                                                            = state<shutting_down>,
            state<idling>                 + event<image_request>                                  / send_calculation_messages{}    = state<calculating>,
            state<calculating>            + event<cancel>                                                                          = state<canceling_calculation>,
            state<calculating>            + event<calculation_results>                            / receive_calculation_results{}  = state<receiving_calculation_results>,
            state<receiving_calculation_results>                        [ has_all_results{} ]     / send_colorization_messages{}   = state<coloring>,
            state<receiving_calculation_results>                        [ has_not_all_results{} ]                                  = state<calculating>,
            state<coloring>               + event<cancel>                                                                          = state<canceling_colorization>,
            state<coloring>               + event<colorization_results>                           / receive_colorization_results{} = state<receiving_colorization_results>,
            state<receiving_colorization_results>                       [ has_all_results{} ]                                      = state<idling>,
            state<receiving_colorization_results>                       [ has_not_all_results{} ]                                  = state<coloring>,
            state<canceling_calculation>  + event<calculation_results>                                                             = state<idling>,
            state<canceling_colorization> + event<colorization_results>                                                            = state<idling>,
            state<shutting_down>                                                                                                   = X
        );
        // clang-format on
    }
};

int main()
{
    supervisor sv;
    my_logger logger;
    sml::sm<state_machine, sml::logger<my_logger>> sm{sv, logger};

    sm.process_event(started{});
    sm.process_event(image_request{4});
    assert(sm.is(sml::state<calculating>));
    sm.process_event(calculation_results{});
    sm.process_event(calculation_results{});
    sm.process_event(calculation_results{});
    sm.process_event(calculation_results{});
    assert(sm.is(sml::state<coloring>));
    sm.process_event(calculation_results{});  // ignore
    sm.process_event(calculation_results{});  // ignore
    sm.process_event(colorization_results{});
    sm.process_event(colorization_results{});
    sm.process_event(colorization_results{});
    sm.process_event(colorization_results{});
    assert(sm.is(sml::state<idling>));
    sm.process_event(colorization_results{});  // ignore
    sm.process_event(colorization_results{});  // ignore
    sm.process_event(quit{});

    assert(sm.is(sml::X));

    dump(sm);
}
