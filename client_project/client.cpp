#include <cstring>
#include <iostream>
#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;
const char *SERVER_IP = "127.0.0.1";

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::cerr << "Ошибка создания сокета\n";
    return 1;
  }

  // 2. настройка адреса сервера
  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

  if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Ошибка подключения к серверу\n";
    return 1;
  }

  std::cout << "Подключен к серверу. Вводите сообщения (exit для выхода):\n";

  while (true) {
    std::cout << "> ";
    std::string message;
    std::getline(std::cin, message);

    // Отправляем
    send(sock, message.c_str(), message.length(), 0);

    if (message == "exit") {
      std::cout << "Отключаемся...\n";
      break;
    }

    // Получаем ответ
    char buffer[1024] = {0};
    int bytes_read = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
      std::cout << "Сервер закрыл соединение\n";
      break;
    }
    buffer[bytes_read] = '\0';
    std::cout << buffer << "\n";
  }

  close(sock);
  return 0;
}