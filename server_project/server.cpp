#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;

int main() {
  // сокет
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == -1) {
    std::cerr << "Ошибка создания сокета\n";
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
             sizeof(opt)); // разрешение на переиспользование порта

  // Привязка порта
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Ошибка bind\n";
    return 1;
  }

  // Начинаем слушать
  if (listen(server_fd, 1) < 0) { // 1 - максимум в очереди
    std::cerr << "Ошибка listen\n";
    return 1;
  }

  std::cout << "Сервер запущен на порту " << PORT << ", ждем клиента...\n";

  // подключение
  int client_fd = accept(server_fd, nullptr, nullptr);
  if (client_fd < 0) {
    std::cerr << "Ошибка accept\n";
    return 1;
  }

  std::cout << "Клиент подключился!\n";

  // чтение сообщения
  char buffer[1024] = {0};
  int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    std::cout << "Получено от клиента: " << buffer << "\n";
  }

  close(client_fd);
  close(server_fd);

  return 0;
}