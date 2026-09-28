#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

// клиент

struct Command {
    std::string massage{};
    Command() : massage(1024, '\0') {}
};

Command client;

class Client {
   private:
    int clientSock;

   public:
    Client() : clientSock(-1) {}

    ~Client() { close(this->clientSock); }

    void StartClient(short port) {
        this->clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (this->clientSock < 0) {
            std::cerr << "Ошибка создания сокета клиента " << strerror(errno);
            exit(1);
        }

        std::cout << "Сокет клиента успешно создан." << std::endl;

        struct sockaddr_in clientAddress{};
        clientAddress.sin_family = AF_INET;
        clientAddress.sin_port = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1", &clientAddress.sin_addr.s_addr) < 0) {
            std::cerr << "Указан неверный IP адрес. " << strerror(errno);
            exit(1);
        }

        socklen_t socklen = sizeof(clientAddress);
        if (connect(this->clientSock, (struct sockaddr*)&clientAddress, socklen) < 0) {
            std::cerr << "Ошибка подключения к серверу " << strerror(errno);
            exit(1);
        }

        std::cout << "Успешное подключение к серверу." << std::endl;
    }
    void send_command() {
        std::cout << "Введите конанду\n";
        while (true) {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);

            try {
                client.massage = input;
                client.massage.resize(client.massage.size());
            } catch (const std::exception& e) {
                std::cout << "Ошибка! Введите целое число.\n";
                continue;
            }

            if (send(this->clientSock, client.massage.c_str(), client.massage.size(), 0) < 0) {
                std::cerr << "Ошибка. Данные не были отправлены" << strerror(errno);
                exit(1);
            }

            std::cout << "Данные отправлены успешно." << std::endl;
            if (recv_command() == 0) {
                break;
            }
        }
    }

    int recv_command() {
        client.massage.resize(1024, '\0');
        std::cout << "Получение ответа от сервера" << std::endl;
        int bite_size = recv(this->clientSock, client.massage.data(), client.massage.size(), 0);
        client.massage.resize(bite_size);

        std::cout << "Получена команда от сервера: " << client.massage << std::endl;

        if (bite_size == 0) {
            std::cout << "Сервер штатно закрыл соединение" << std::endl;
            return 0;
        } else if (bite_size == -1) {
            std::cerr << "Возникла ошибка при принятии данных от сервера" << strerror(errno);
            exit(1);
        }
        return 1;
    }
};

int main(int argc, char* argv[]) {
    Client client;

    client.StartClient(std::stoi(argv[1]));
    client.send_command();

    return 0;
}