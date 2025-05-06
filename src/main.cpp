#include <fstream>
#include <iostream>
#include <string>

#include "events.h"
#include "event_parser.h"

int main(const int argc, const char** argv) {
    if (argc != 2) {
        std::cerr << "Incorrect main arguments, usage: task <file.txt>\n";
        return 1;
    }
    event_parser parser(argv[1]);
    state s = parser.parse_beginning();
    auto events = parser.parse();
    event::process(events, s);
    return 0;
}
