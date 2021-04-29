#include <cassert>
#include <vector>

#include <boost/sml.hpp>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace sml = boost::sml;

struct data {
    std::vector<int> values_;
};

struct waiting {};
struct printing {};

struct add {
    int n_;
};

auto has_three_values = [](const data& d) {
    spdlog::info("[guard \"has_three_values\"] values: {}", d.values_.size());
    return d.values_.size() == 3;
};

auto add_value = [](const add& e, data& d) {
    spdlog::info("[action \"add_value\"] add {}", e.n_);
    d.values_.push_back(e.n_);
};

auto print_values = [](const data& d) {
    spdlog::info("[action \"add_value\"] printing values: {}", d.values_);
};

struct state_machine {
    auto operator()()
    {
        using namespace sml;

        return make_transition_table(
          * state<waiting>  + event<add> [  has_three_values ]                = state<printing>,
            state<waiting>  + event<add> [ !has_three_values ] / add_value,
            state<printing>                                    / print_values = X
        );
    }
};

int main()
{
    data d;
    sml::sm<state_machine> sm{d};

    sm.process_event(add{1});  // one value, stay in "waiting"
    assert(d.values_.size() == 1);
    assert(sm.is(sml::state<waiting>));
    sm.process_event(add{2});  // two values, stay in "waiting"
    assert(d.values_.size() == 2);
    assert(sm.is(sml::state<waiting>));
    sm.process_event(add{3});  // three values, transition to "printing" and immediately to "X"
    assert(d.values_.size() == 3);
    sm.process_event(add{4});  // ignored
    sm.process_event(add{5});  // ignored

    assert(d.values_.size() == 3);
    assert(sm.is(sml::X));
}
