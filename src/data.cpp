#include <cassert>
#include <iostream>
#include <string>

#include <boost/sml.hpp>

namespace sml = boost::sml;

struct connect {
    int id{};
};
struct disconnect {};
struct interrupt {};

struct Disconnected {};
struct Connected {
    int id{}; // per state data
};
struct Interrupted {
    int id{}; // per state data
};

class Data {
    using self = Data;

public:
    explicit Data(const std::string& address) : address{address} { }

    auto operator()()
    {
        using namespace boost::sml;

        const auto set = [](const auto& event, Connected& state) { state.id = event.id; };
        const auto update = [](Connected& src_state, Interrupted& dst_state) { dst_state.id = src_state.id; };

        // clang-format off
        return make_transition_table(
          * state<Disconnected> + event<connect>    / (set,    &self::print) = state<Connected>,
            state<Connected>    + event<interrupt>  / (update, &self::print) = state<Interrupted>,
            state<Interrupted>  + event<connect>    / (set,    &self::print) = state<Connected>,
            state<Connected>    + event<disconnect> / (        &self::print) = X
        );
        // clang-format on
    }

private:
    void print(Connected& state) { std::cout << address << ':' << state.id << '\n'; };

    std::string address{}; // shared data between states
};

int main()
{
    Data d{std::string{"127.0.0.1"}};
    sml::sm<Data> sm{d, Connected{1}};

    sm.process_event(connect{1024});
    sm.process_event(interrupt{});
    sm.process_event(connect{1025});
    sm.process_event(disconnect{});

    assert(sm.is(sml::X));
}
