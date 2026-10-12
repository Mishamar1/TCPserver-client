#include "../libs/pars_http.h"

using namespace HTTP21;

HTTPrequest HTTP21::parse_http(const std::string& raw, const char delimiter) {
    HTTP21::HTTPrequest req;
    size_t start = 0;

    size_t end = raw.find("\r\n", start);
    std::string line = raw.substr(start, end);

    size_t space1 = line.find(' ');
    size_t space2 = line.rfind(' ') + 1;
    size_t slesh = line.find('/', space2 + 1);

    req.metod = line.substr(start, space1);
    req.url = line.substr(space2, slesh - space2);
    req.version = line.substr(slesh + 1, end - slesh);

    start = end + 2;

    while (start < raw.size() - 4) {
        end = raw.find("\r\n", start);
        if (end == std::string::npos || end == start) break;

        line = raw.substr(start, end - start);
        size_t colon = line.find(':');

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        size_t vstart = value.find_first_not_of(" \t");
        if (vstart != std::string::npos) {
            value = value.substr(vstart);
        }

        req.headers.emplace(key, value);
        start = end + 2;
    }

    return req;
}
