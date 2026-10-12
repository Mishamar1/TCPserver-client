#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string s{
        "GET / HTTP/1.1\r\n Host: localhost:8080\r\n User-Agent: curl/8.18.0\r\n Accept: */*\r\n\r\n"};

    size_t end = s.find("\r\n");
    std::cout << end << std::endl;
    // std::string big_data{"CiscoToll\r\n\r\n\r\n"};
    // if (big_data.rfind("\r\r\r\n") != std::string::npos) {
    //     std::cout << "Найден конец сообщения. Сообщение получено полность!\n";
    // } else {
    //     std::cout << "Конец сообщения не обнаружен. Продождаю поиск...\n";
    // }

    // std::cout << big_data << std::endl;

    return 0;
}