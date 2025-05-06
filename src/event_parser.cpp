#include "event_parser.h"
#include "state-utils.h"
#include "events.h"

#include <string>
#include <vector>
#include <sstream>

event_parser::event_parser(const std::string& filename) : _in(filename) {
    if (!_in.is_open()) {
        throw simulation_exception("Could not read from file: '" + filename + "'!");
    }
}

state event_parser::parse_beginning() {
    std::vector<std::string> tmp = read_line();
    throw_error_if(tmp.size() != 1);
    std::size_t tables = str_to_int(tmp[0]);

    tmp = read_line();
    throw_error_if(tmp.size() != 2);
    uint16_t open_time = str_to_time(tmp[0]);
    uint16_t close_time = str_to_time(tmp[1]);

    tmp = read_line();
    throw_error_if(tmp.size() != 1);
    std::size_t earn_h = str_to_int(tmp[0]);

    return {tables, earn_h, open_time, close_time};
}


std::vector<std::unique_ptr<event>> event_parser::parse() {
    std::vector<std::unique_ptr<event>> events;
    while (!_in.eof()) {
        events.push_back(parse_event());
    }
    return events;
}

std::unique_ptr<event> event_parser::parse_event() {
    std::vector<std::string> line = read_line();
    throw_error_if(line.size() < 2);

    uint16_t time = str_to_time(line[0]);
    std::size_t id = str_to_int(line[1]);

    if (id == IN_CLIENT_ARRIVE) {
        throw_error_if(line.size() != 3);
        std::string name = str_to_name(line[2]);
        return std::make_unique<client_arrive_event>(time, std::move(name));

    } else if (id == IN_CLIENT_LEAVE) {
        throw_error_if(line.size() != 3);
        std::string name = str_to_name(line[2]);
        return std::make_unique<client_leave_event>(time, std::move(name));

    } else if (id == IN_CLIENT_SAT) {
        throw_error_if(line.size() != 4);
        std::string name = str_to_name(line[2]);
        std::size_t table = str_to_int(line[3]);
        return std::make_unique<client_sat_event>(time, std::move(name), table);

    } else if (id == IN_CLIENT_WAIT) {
        throw_error_if(line.size() != 3);
        std::string name = str_to_name(line[2]);
        return std::make_unique<client_wait_event>(time, std::move(name));
    } else {
        throw_error();
    }

    return {};
}


std::vector<std::string> event_parser::read_line() {
    std::string s;
    std::getline(_in, s);
    ++_line;

    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string tmp;
    while (ss >> tmp) {
        res.push_back(tmp);
    }
    return res;
}

uint16_t event_parser::str_to_time(const std::string &s) const {
    if (s.size() != 5 || s[2] != ':') {
        throw_error_if(true);
    }
    uint16_t h = 0;
    uint16_t m = 0;
    try {
        h = std::stoi(s.substr(0, 2));
        m = std::stoi(s.substr(3, 2));
    } catch (...) {
        throw_error_if(true);
    }
    throw_error_if(h < 0 || h > 23 || m < 0 || m > 59);
    return h * 60 + m;
}

void event_parser::throw_error_if(const bool cond) const {
    if (cond) {
        throw simulation_exception("File error on line: '" + std::to_string(_line) + "'!");
    }
}

void event_parser::throw_error() const {
    throw_error_if(true);
}


int event_parser::str_to_int(const std::string &s) const {
    int x = 0;
    try {
        x = stoi(s);
    } catch (...) {
        throw_error_if(true);
    }
    return x;
}

std::string event_parser::str_to_name(const std::string &s) const {
    bool allowed = true;
    for (const auto c : s) {
        allowed = allowed && ('0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z' || c == '_');
    }
    throw_error_if(!allowed);
    return s;
}




