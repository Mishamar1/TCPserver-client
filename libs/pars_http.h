#ifndef __PARS_HTTP__
#define __PASR_HTTP__
#include <cstring>
#include <iostream>
#include <map>
#include <string>

namespace HTTP21 {
struct HTTPrequest {
    std::string metod;                                // метод запроса
    std::string url;                                  // url сайта
    std::string version;                              // версия HTTP
    std::multimap<std::string, std::string> headers;  // данные запроса
};

HTTPrequest parse_http(const std::string& str, const char delimiter);
}  // namespace HTTP21
#endif