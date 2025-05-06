#pragma once

#include "state-utils.h"
#include "events.h"

#include <fstream>
#include <memory>

class event_parser {
public:
    event_parser(const std::string& filename);

    std::vector<std::unique_ptr<event>> parse();

    state parse_beginning();

private:
    std::unique_ptr<event> parse_event();

    std::string str_to_name(const std::string& s) const;

    uint16_t str_to_time(const std::string& s) const;

    int str_to_int(const std::string& s) const;

    std::vector<std::string> read_line();

    void throw_error_if(bool cond) const;

    void throw_error() const;

    std::ifstream _in;
    std::size_t _line = 0;
};
