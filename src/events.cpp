#include "events.h"

#include <iostream>

event::event(const uint16_t t) : _time(t) {}

uint16_t event::time() const noexcept {
    return _time;
}

client_arrive_event::client_arrive_event(uint16_t time, std::string name) : event(time) {
    _client_name = std::move(name);
}

client_wait_event::client_wait_event(uint16_t time, std::string name) : event(time) {
    _client_name = std::move(name);
}

client_leave_event::client_leave_event(uint16_t time, std::string name) : event(time) {
    _client_name = std::move(name);
}

client_sat_event::client_sat_event(uint16_t time, std::string name, std::size_t table) : event(time) {
    _client_name = std::move(name);
    _table = table;
}

auto_client_leave_event::auto_client_leave_event(uint16_t time, std::string name) : event(time) {
    _client_name = std::move(name);
}

auto_client_sat_event::auto_client_sat_event(uint16_t time, std::string name, std::size_t table) : event(time) {
    _client_name = std::move(name);
    _table = table;
}

error_event::error_event(uint16_t time, std::string msg) : event(time) {
    _msg = std::move(msg);
}

std::string client_wait_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(IN_CLIENT_WAIT) + " " + _client_name;
}

std::string client_arrive_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(IN_CLIENT_ARRIVE) + " " + _client_name;
}

std::string client_leave_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(IN_CLIENT_LEAVE) + " " + _client_name;
}

std::string client_sat_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(IN_CLIENT_SAT) + " " + _client_name + " " + std::to_string(_table);
}

std::string auto_client_leave_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(OUT_CLIENT_LEAVE) + " " + _client_name;
}

std::string auto_client_sat_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(OUT_CLIENT_SAT) + " " + _client_name + " " + std::to_string(_table);
}

std::string error_event::to_string() const noexcept {
    return time_to_string(time()) + " " + std::to_string(OUT_ERROR) + " " + _msg;
}

std::unique_ptr<event> client_arrive_event::apply(state &state) {
    state.update_time(time());
    if (state.is_registered(_client_name)) {
        return std::make_unique<error_event>(time(), "YouShallNotPass");
    }
    if (time() <state.open_time  || state.close_time < time()) {
        return std::make_unique<error_event>(time(), "NotOpenYet");
    }
    state.register_client(_client_name);
    return nullptr;
}

std::unique_ptr<event> client_leave_event::apply(state &state) {
    state.update_time(time());
    std::unique_ptr<event> res = nullptr;
    if (!state.is_registered(_client_name)) {
        return std::make_unique<error_event>(time(), "ClientUnknown");
    }
    client& c = state.registered[_client_name];
    if (c.sitting()) {
        std::size_t table = c.table;
        state.remove_from_play(c.name);
        if (state.has_next_in_queue()) {
            res = std::make_unique<auto_client_sat_event>(time(), state.queue.front(), table);
        }
    }
    state.unregister_client(_client_name);
    return res;
}

std::unique_ptr<event> client_sat_event::apply(state &state) {
    state.update_time(time());
    if (!state.tables[_table].is_free) {
        return std::make_unique<error_event>(time(), "PlaceIsBusy");
    }
    if (!state.is_registered(_client_name)) {
        return std::make_unique<error_event>(time(), "ClientUnknown");
    }
    state.add_to_play(_client_name, _table);
    return nullptr;
}

std::unique_ptr<event> client_wait_event::apply(state &state) {
    state.update_time(time());
    if (state.free_tables != 0) {
        return std::make_unique<error_event>(time(), "ICanWaitNoLonger!");
    }
    if (state.in_queue > state.tables.size()) {
        return std::make_unique<auto_client_leave_event>(time(), _client_name);
    }
    state.add_to_queue(_client_name);
    return nullptr;
}

std::unique_ptr<event> auto_client_leave_event::apply(state &state) {
    state.update_time(time());
    client& c = state.registered[_client_name];
    if (c.sitting()) {
        state.remove_from_play(c.name);
    }
    // if he is in queue
    state.unregister_client(c.name);
    return nullptr;
}

std::unique_ptr<event> auto_client_sat_event::apply(state &state) {
    state.update_time(time());
    state.remove_from_queue(_client_name);
    state.add_to_play(_client_name, _table);
    return nullptr;
}

std::unique_ptr<event> error_event::apply(state &state) {
    state.update_time(time());
    return nullptr;
}

std::string event::time_to_string(uint16_t time) {
    if (time > 23 * 60 + 59) {
        throw simulation_exception("Incorrect time given.");
    }
    std::size_t h = time / 60;
    std::size_t m = time % 60;
    std::string res;
    if (h < 10) {
        res.push_back('0');
    }
    res += std::to_string(h);
    res.push_back(':');
    if (m < 10) {
        res.push_back('0');
    }
    res += std::to_string(m);
    return res;
}


void event::process(const std::vector<std::unique_ptr<event> > &events, state &s) {
    std::cout << time_to_string(s.open_time) << "\n";

    for (auto& ptr : events) {
        auto new_event = ptr->apply(s);
        std::cout << ptr->to_string() << "\n";
        if (new_event != nullptr) {
            std::cout << new_event->to_string() << "\n";
        }
    }
    for (auto&[name, c] : s.registered) {
        if (c.sitting()) {
            auto e = std::make_unique<auto_client_leave_event>(s.close_time, name);
            e->apply(s);
            std::cout << e->to_string() << "\n";
        }
    }

    std::cout << time_to_string(s.close_time) << "\n";

    for (std::size_t i=0; i<s.tables.size(); ++i) {
        std::cout << (i + 1) << " " << s.tables[i].load_time * s.earn_in_h << " " << time_to_string(s.tables[i].load_time) << "\n";
    }
}

