#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "../libs/pars_http.h"

// сервер

struct Command {
    std::string commandType{};
    Command() : commandType(65536, '\0') {}
};

std::mutex mtx_client;

void handle_client(int client_fd) {
    // функция для обработки каждого киента в отдельном потоке
    {
        std::lock_guard<std::mutex> lockend(mtx_client);
        std::cout << "Клиент подключился. (Поток: " << std::this_thread::get_id() << ")\n";
    }

    Command command;

    while (true) {
        char buf[1024];
        int bytes = recv(client_fd, buf, 1024 - 1, 0);
        buf[bytes] = '\0';
        std::string mini_data(buf, bytes);

        if (bytes > 0) {
            command.commandType += mini_data;
        } else {
            std::lock_guard<std::mutex> lockend(mtx_client);
            if (bytes == 0) {
                std::cout << "Клиент штатно закрыл соединение. (Поток: " << std::this_thread::get_id() << ")"
                          << std::endl;
            } else {
                std::cout << "Ошибка при прочтении данных от клиента. Данные не прочитаны. (Поток: "
                          << std::this_thread::get_id() << ") " << strerror(errno);
            }

            break;
        }
        // if (buf.rfind("\r\n\r\n") != std::string::npos) {
        //     std::lock_guard<std::mutex> lockend(mtx_client);
        //     std::cout << "> Данные успешно прочитаны!\nПоступило сообщение:\n" << command.commandType <<
        //     "\n"; break;
        // }
    }

    {
        std::lock_guard<std::mutex> lokend1(mtx_client);
        std::cout << "> Данные успешно прочитаны!\nПоступило сообщение:\n" << command.commandType << "\n";
        std::cout << "Поток: " << std::this_thread::get_id() << "\n";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //*************************************************************************************************//
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // ****************************** РАБОТА С ПОЛУЧЕННЫМИ ДАННЫМИ ************************************//
    /////////////////////////////////////////////////////////////////////////////////////////////////////
    //*************************************************************************************************//
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    // if (send(client_fd, command.commandType.c_str(), command.commandType.size(), 0) < 0) {
    //     {
    //         std::lock_guard<std::mutex> lockend(mtx_client);
    //         std::cout << "Ошибка отправки данных клиенту. (Поток: " << std::this_thread::get_id() << ")
    //         "
    //                   << strerror(errno);
    //     }
    // }
    // {
    //     std::lock_guard<std::mutex> lockend(mtx_client);
    //     std::cout << "Данные успешно отправлены клиенту! Команда: " << command.commandType
    //               << " (Поток: " << std::this_thread::get_id() << ")\n";
    // }

    // parse_http(command.commandType, command.commandType.length());

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // *************************** КОНЕЦ РАБОТЫ С ПОЛУЧЕННЫМИ ДАННЫМИ *********************************//
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    close(client_fd);
    {
        std::lock_guard<std::mutex> lockend(mtx_client);
        std::cout << "Клиент успешно бработан и закрыт! (Поток: " << std::this_thread::get_id() << ")"
                  << std::endl;
    }
}

class Server {
   private:
    int serverSock;
    bool runing;

   public:
    Server() : serverSock(-1), runing(false) {}
    ~Server() {
        if (runing) close_command();
    }

    void StartServer() {
        this->serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (this->serverSock < 0) {
            std::cerr << "Ошибка создания сокета\n";
            exit(1);
        }

        struct sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        // serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr.s_addr) < 0) {
            std::cerr << "Указан неверный IP адрес. " << strerror(errno);
            exit(1);
        }

        if (bind(this->serverSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Ошибка привязки сокета к адресу и порту. " << strerror(errno);
            exit(1);
        }

        std::cout << "Сервер запущен к порту: " << ntohs(serverAddress.sin_port) << "\n";

        if (listen(this->serverSock, 1) < 0) {
            std::cerr << "Ошибка запуска прослушивания входящих соединений. " << strerror(errno);
            exit(1);
        }

        std::cout << "Ожидание подключения клиента..." << std::endl;

        this->runing = true;
    }

    void run() {
        while (this->runing) {
            struct sockaddr_in clientAddress{};
            socklen_t clientAddressLen = sizeof(clientAddress);

            int clientSock = accept(this->serverSock, (struct sockaddr*)&clientAddress, &clientAddressLen);
            if (clientSock < 0) {
                if (this->runing == false)
                    std::cerr << "Ошибка при принятии входящего соединения. " << strerror(errno);
                continue;
            }
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddress.sin_addr, client_ip, INET_ADDRSTRLEN);
            std::cout << "Подключен новый клиент: " << client_ip << "\t:\t" << ntohs(clientAddress.sin_port)
                      << std::endl;

            std::thread client_thread(handle_client, clientSock);
            client_thread.detach();
        }
    }

    void close_command() {
        this->runing = false;
        shutdown(this->serverSock, 2);

        if (this->serverSock != -1) {
            close(this->serverSock);
            this->serverSock = -1;
        }
    }
};

int main(int argc, char* argv[]) {
    Server server;
    server.StartServer();
    server.run();

    return 0;
}