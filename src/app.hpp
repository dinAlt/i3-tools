#ifndef I3_FOCUS_LAST_APP
#define I3_FOCUS_LAST_APP

#include <memory>
#include <mutex>
#include <string_view>

#include "i3ipc++/ipc.hpp"
#include "server.hpp"

using cont_list = std::list<std::shared_ptr<i3ipc::container_t>>;

class App {
public:
  App() noexcept;
  void execute(const std::vector<char *> args);
  void exit() noexcept;

private:
  const static int HISTORY_LEN = 10;
  inline static std::string SOCKET_NAME = "socket";

  std::unique_ptr<Server> server{};
  std::unique_ptr<i3ipc::connection> i3con{};
  std::mutex mut{};
  std::vector<uint64_t> history{};

  void exec_server();
  void exec_switch_window();
  void exec_help();
  std::optional<i3ipc::container_t> i3_find_scratchpad() noexcept;
  void on_server_message(std::string_view message) noexcept;
  void on_os_signal(int signum) noexcept;
  void on_i3_event(const i3ipc::window_event_t &evt) noexcept;
  void stop() noexcept;
};
#endif
