#include "../libs/pars_http.h"

#include <cstring>
#include <iostream>
#include <string>

void parse_http(const std::string& data, size_t len) {
    std::string buf;
    size_t i = 0;

    while (i < len) {
        buf += data[i];
        if (data[i] == ':') {
            std::cout << buf << " : ";
        } else if (data[i] == '\r') {
            std::cout << '\n';
        }
        i++;
    }
}
