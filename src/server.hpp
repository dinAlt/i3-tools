#ifndef I3_FOCUS_LAST_SERVER
#define I3_FOCUS_LAST_SERVER

#include <functional>
#include <string>
#include <string_view>

typedef std::function<void(std::string_view)> message_handler;

class Server {
public:
  Server(std::string socket_path) noexcept;
  ~Server() noexcept;
  void listen_and_serve(message_handler cb);
  void stop() noexcept;

private:
  std::string socket_path{};

  int socket_fd{};
  message_handler on_message{};

  void serve();
  void create_socket();
};
#endif
