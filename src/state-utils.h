#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <queue>

struct client {
    client() = default;

    client(const std::string& _name);
    client(const std::string& _name, std::size_t _table);

    client(const client&) = default;
    client(client&&) = default;
    client& operator=(const client&) = default;
    client& operator=(client&&) = default;

    void sit(std::size_t t);

    void stand();

    [[nodiscard]] bool sitting() const noexcept;

    std::string name;
    std::size_t table = -1;
};

struct table {
    void load(uint16_t time);

    void free(uint16_t time);

    uint16_t last_busy_time = 0;
    uint16_t load_time = 0;
    bool is_free = true;
};

class state {
public:
    state() = default;

    state(std::size_t tables, std::size_t earn_h, uint16_t open_time, uint16_t close_time);

    state(const state&) = default;
    state(state&&) = default;
    state& operator=(const state&) = default;
    state& operator=(state&&) = default;

    bool is_registered(const std::string& name);

    void add_to_play(const std::string& name, std::size_t place);

    void add_to_queue(const std::string& name);

    void register_client(const std::string& name);

    void remove_from_queue(const std::string& name);

    void remove_from_play(const std::string& name);

    void unregister_client(const std::string& name);

    std::string next_in_queue();

    bool has_next_in_queue();

    void update_time(uint16_t new_time);

    std::map<std::string, client> registered;
    std::queue<std::string> queue;
    std::set<std::string> removed_from_queue;
    std::vector<table> tables;

    std::size_t earn_in_h = 0;
    std::size_t free_tables;
    std::size_t in_queue = 0;
    uint16_t open_time;
    uint16_t close_time;
    uint16_t cur_time = 0;
};

class simulation_exception : public std::exception {
public:
    explicit simulation_exception(std::string  msg);

    [[nodiscard]] const char* what() const noexcept override;

private:
    std::string _msg;
};
