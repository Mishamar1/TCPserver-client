#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// сервер

struct Command {
  int commandType = 1;
};

Command command;

class Server {
private:
  int serverSock;
  int clientSock;

public:
  Server() : serverSock(-1), clientSock(-1) {}
  ~Server() {}

  void StartServer() {
    this->serverSock = socket(AF_INET, SOCK_STREAM, 0);
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

    if (bind(this->serverSock, (struct sockaddr *)&serverAddress,
             sizeof(serverAddress)) < 0) {
      std::cerr << "Ошибка привязки сокета к адресу и порту. "
                << strerror(errno);
      exit(1);
    }

    std::cout << "Сервер запущен." << std::endl;

    if (listen(this->serverSock, 1) < 0) {
      std::cerr << "Ошибка запуска прослушивания входящих соединений. "
                << strerror(errno);
      exit(1);
    }

    std::cout << "Ожидание подключения клиента к порту: "
              << ntohs(serverAddress.sin_port) << std::endl;

    struct sockaddr_in clientAddress{};
    socklen_t clientAddressLen = sizeof(clientAddress);

    this->clientSock = accept(
        this->serverSock, (struct sockaddr *)&clientAddress, &clientAddressLen);
    if (this->clientSock < 0) {
      std::cerr << "Ошибка при принятии входящего соединения. "
                << strerror(errno);
      exit(1);
    }

    std::cout << "Клиет успешно подключен!" << std::endl;
  }

  void send_command() {
    if (send(this->clientSock, &command.commandType,
             sizeof(command.commandType), 0) < 0) {
      std::cerr << "Ошибка при отправке данных клиенту. " << strerror(errno);
      exit(1);
    }
    std::cout << "Данные успешно отправлены клиенту!" << std::endl;
  }

  void recv_command() {
    if (recv(this->clientSock, &command.commandType,
             sizeof(command.commandType), 0) < 0) {
      std::cerr
          << "Ошибка при прочтении данных от клиента. Данные не прочитаны... "
          << strerror(errno);
      exit(1);
    }

    std::cout << "Данные успешно прочитаны!\nПоступила команда: "
              << command.commandType << std::endl;
  }

  void close_command() {
    shutdown(this->serverSock, 2);
    shutdown(this->clientSock, 2);

    if (this->serverSock != -1) {
      close(this->serverSock);
      this->serverSock = -1;
    }
    if (this->clientSock != -1) {
      close(this->clientSock);
      this->clientSock = -1;
    }
  }
};

int main(int argc, char *argv[]) {
  Server server;
  server.StartServer();
  server.recv_command();
  // server.send_command();

  server.close_command();

  return 0;
}