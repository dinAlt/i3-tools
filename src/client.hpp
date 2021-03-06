#ifndef I3_TOOLS_CLIENT
#define I3_TOOLS_CLIENT

#include <string>

class Client {
public:
  Client(std::string socket_path) noexcept;
  ~Client() noexcept;
  void connect();
  void write(std::string_view message);

private:
  std::string socket_path{};

  int socket_fd{};
  void create_socket();
};
#endif
