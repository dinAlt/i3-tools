#include "client.hpp"
#include <cerrno>
#include <iostream>
#include <ostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <system_error>
#include <unistd.h>

// public members

Client::Client(std::string socket_path) noexcept : socket_path{socket_path} {}

Client::~Client() noexcept {
  if (socket_fd < 1) {
    return;
  }

  close(socket_fd);
  socket_fd = 0;
}

void Client::connect() {
  const int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd == -1) {
    throw std::system_error(errno, std::generic_category());
  }
  socket_fd = fd;

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_LOCAL;
  strncpy(addr.sun_path, socket_path.data(), sizeof(addr.sun_path) - 1);

  const int res =
      ::connect(fd, (const struct sockaddr *)&addr, sizeof(struct sockaddr_un));
  if (res == -1) {
    throw std::system_error(errno, std::generic_category());
  }
}

void Client::write(std::string_view message) {
  if (::write(socket_fd, message.data(), message.size()) == -1) {
    throw std::system_error(errno, std::generic_category());
  }
}
