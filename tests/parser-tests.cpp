#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include "../src/event_parser.h"
#include "../src/events.h"

TEST(parser, base) {
    event_parser parser("../tests/samples/sample.txt");
    state s = parser.parse_beginning();
    ASSERT_EQ(s.tables.size(), 3);
    ASSERT_EQ(s.open_time, 9 * 60);
    ASSERT_EQ(s.close_time, 19 * 60);
    ASSERT_EQ(s.earn_in_h, 10);
    auto events = parser.parse();
    ASSERT_EQ(events.size(), 14);
}

TEST(parser, incorrect_begin) {
    {
        event_parser parser("../tests/samples/incorrect1.txt");
        ASSERT_ANY_THROW(parser.parse_beginning());
    }
    {
        event_parser parser("../tests/samples/incorrect2.txt");
        ASSERT_ANY_THROW(parser.parse_beginning());
    }
}

TEST(parser, incorrect_events) {
    {
        event_parser parser("../tests/samples/incorrect3.txt");
        parser.parse_beginning();
        ASSERT_ANY_THROW(parser.parse());
    }
    {
        event_parser parser("../tests/samples/incorrect4.txt");
        parser.parse_beginning();
        ASSERT_ANY_THROW(parser.parse());
    }
}
