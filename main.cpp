#include <iostream>
#include <sstream>
#include <string>

int main() {
    const char* arr = "qwerty123456\0";
    std::string data(arr, 200);

    std::cout << data << std::endl;
    // std::string big_data{"CiscoToll\r\n\r\n\r\n"};
    // if (big_data.rfind("\r\r\r\n") != std::string::npos) {
    //     std::cout << "Найден конец сообщения. Сообщение получено полность!\n";
    // } else {
    //     std::cout << "Конец сообщения не обнаружен. Продождаю поиск...\n";
    // }

    // std::cout << big_data << std::endl;

    return 0;
}