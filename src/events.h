#pragma once

#include "state-utils.h"

#include <string>
#include <memory>

enum event_types {
    IN_CLIENT_ARRIVE = 1,
    IN_CLIENT_SAT = 2,
    IN_CLIENT_WAIT = 3,
    IN_CLIENT_LEAVE = 4,
    OUT_CLIENT_LEAVE = 11,
    OUT_CLIENT_SAT = 12,
    OUT_ERROR = 13
};

class event {
public:
    explicit event(uint16_t time);

    virtual std::unique_ptr<event> apply(state& s) = 0;

    virtual ~event() = default;

    [[nodiscard]] virtual std::string to_string() const noexcept = 0;

    [[nodiscard]] uint16_t time() const noexcept;

    static void process(const std::vector<std::unique_ptr<event>>& events, state& s);

    static std::string time_to_string(uint16_t time);
private:
    uint16_t _time;
};

class client_arrive_event : public event {
public:
    explicit client_arrive_event(uint16_t time, std::string name);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
};

class client_sat_event : public event {
public:
    explicit client_sat_event(uint16_t time, std::string name, std::size_t table);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
    std::size_t _table;
};

class client_wait_event : public event {
public:
    explicit client_wait_event(uint16_t time, std::string name);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
};

class client_leave_event : public event {
public:
    explicit client_leave_event(uint16_t time, std::string name);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
};

class auto_client_leave_event : public event {
public:
    explicit auto_client_leave_event(uint16_t time, std::string name);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
};

class auto_client_sat_event : public event {
public:
    explicit auto_client_sat_event(uint16_t time, std::string name, std::size_t table);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;
private:
    std::string _client_name;
    std::size_t _table;
};

class error_event : public event {
public:
    explicit error_event(uint16_t time, std::string msg);

    std::unique_ptr<event> apply(state& state) override;

    [[nodiscard]] std::string to_string() const noexcept override;

private:
    std::string _msg;
};
