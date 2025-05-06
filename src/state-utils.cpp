#include "state-utils.h"

#include <utility>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

simulation_exception::simulation_exception(std::string msg) : _msg(std::move(msg)) {}

const char *simulation_exception::what() const noexcept {
    return _msg.c_str();
}

client::client(const std::string& _name, std::size_t _table)
    : name(_name), table(_table) {}

client::client(const std::string& _name) : client(_name, 0) {}

bool client::sitting() const noexcept {
    return table == -1;
}

void client::sit(std::size_t t) {
    table = t;
}

void client::stand() {
    table = -1;
}

state::state(const std::size_t _tables, const std::size_t _earn_in_h, uint16_t _open_time, uint16_t _close_time)
    : tables(_tables), earn_in_h(_earn_in_h), free_tables(tables.size()), open_time(_open_time), close_time(_close_time) {}

void table::load(const uint16_t time) {
    if (!is_free) {
        throw simulation_exception("Table is already loaded!");
    }
    is_free = false;
    last_busy_time = time;
}

void table::free(const uint16_t time) {
    if (is_free) {
        throw simulation_exception("Table is already free!");
    }
    is_free = false;
    load_time += time - last_busy_time;
}

void state::update_time(uint16_t new_time) {
    cur_time = new_time;
}

void state::add_to_play(const std::string &name, std::size_t place) {
    tables[place].load(cur_time);
    --free_tables;
}

void state::add_to_queue(const std::string &name) {
    queue.push(name);
    ++in_queue;
}

void state::register_client(const std::string &name) {
    registered[name] = client(name);
}

void state::unregister_client(const std::string &name) {
    registered.erase(name);
}

bool state::is_registered(const std::string &name) {
    return registered.find(name) != registered.end();
}

void state::remove_from_play(const std::string &name) {
    client& c = registered[name];
    tables[c.table].free(cur_time);
    c.stand();
    ++free_tables;
}

void state::remove_from_queue(const std::string &name) {
    removed_from_queue.insert(name);
    --in_queue;
}

std::string state::next_in_queue() {
    while (removed_from_queue.find(queue.front()) != removed_from_queue.end()) {
        removed_from_queue.erase(queue.front());
        queue.pop();
    }
    return queue.front();
}

bool state::has_next_in_queue() {
    while (!queue.empty() && removed_from_queue.find(queue.front()) != removed_from_queue.end()) {
        removed_from_queue.erase(queue.front());
        queue.pop();
    }
    return in_queue != 0;
}
