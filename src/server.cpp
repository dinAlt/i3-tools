#include "server.hpp"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <system_error>

#include <cerrno>
#include <string>
#include <unistd.h>

// public members

Server::Server(std::string socket_path) noexcept : socket_path{socket_path} {};

Server::~Server() noexcept { stop(); }

void Server::listen_and_serve(message_handler on_message) {
  try {
    create_socket();
    listen(socket_fd, 1);
    this->on_message = on_message;
    serve();
  } catch (...) {
    stop();
    throw;
  }
}

void Server::serve() {
  const int fd = socket_fd;

  int n;
  char buffer[256];

  while (true) {
    const int newfd = accept(fd, NULL, NULL);
    bzero(buffer, 256);
    n = read(newfd, buffer, 255);

    if (n < 0) {
      throw std::system_error(errno, std::generic_category());
    }

    on_message(buffer);
    close(newfd);
  }
}

void Server::stop() noexcept {
  if (socket_fd < 1) {
    return;
  }

  close(socket_fd);
  socket_fd = 0;
}

// private members

void Server::create_socket() {
  if (remove(socket_path.data()) == -1 && errno != ENOENT) {
    throw std::system_error(errno, std::generic_category());
  }

  const int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
  if (fd == -1) {
    throw std::system_error(errno, std::generic_category());
  }
  socket_fd = fd;

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_LOCAL;
  strncpy(addr.sun_path, socket_path.data(), sizeof(addr.sun_path) - 1);

  if (bind(fd, (struct sockaddr *)&addr, sizeof(addr))) {
    throw std::system_error(errno, std::generic_category());
  }
}
