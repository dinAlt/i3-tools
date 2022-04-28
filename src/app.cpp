#include "app.hpp"
#include "client.hpp"

#include <algorithm>
#include <cstring>
#include <exception>
#include <filesystem>
#include <future>
#include <i3ipc++/ipc.hpp>
#include <iostream>
#include <string_view>
#include <thread>
#include <unistd.h>

// public members

App::App() noexcept {};

std::filesystem::path get_socket_dir() {
  return std::filesystem::temp_directory_path() /
         ("i3_tools." + std::to_string(getuid()) + std::getenv("DISPLAY"));
}

void App::execute(const std::vector<char *> args) {
  if (args.size() < 2) {
    exec_server();
    return;
  }

  if (strcmp(args[1], "--switch-window") == 0) {
    exec_switch_window();
    return;
  }

  exec_help();
}

void App::exit() noexcept {
  if (server == nullptr) {
    return;
  }

  stop();
}

// private members

void App::exec_server() {
  namespace fs = std::filesystem;
  const fs::path socket_dir = get_socket_dir();
  fs::create_directories(socket_dir);
  fs::permissions(socket_dir, fs::perms::group_all, fs::perm_options::remove);
  fs::permissions(socket_dir, fs::perms::others_all, fs::perm_options::remove);

  server = std::make_unique<Server>(socket_dir / App::SOCKET_NAME);
  i3con = std::make_unique<i3ipc::connection>();

  i3con->subscribe(i3ipc::ET_WINDOW);
  i3con->signal_window_event.connect([&](const i3ipc::window_event_t &evt) {
    mut.lock();
    on_i3_event(evt);
    mut.unlock();
  });
  std::thread i3ipc_thr([&]() {
    while (true) {
      i3con->handle_event();
    }
  });

  i3ipc_thr.detach();
  server->listen_and_serve([&](auto msg) {
    mut.lock();
    on_server_message(msg);
    mut.unlock();
  });
}

void App::exec_switch_window() {
  Client cli(get_socket_dir() / App::SOCKET_NAME);
  cli.connect();
  cli.write("switch");
}

void App::exec_help() {
  std::cout << "usage: i3-tools [-h] [--switch-window]\n\n"
            << "Focus previous focused window for i3wm.\n\n"
            << "This app should be launched with i3wm without options\n"
            << "and then use you may use it with --switch-window option.\n\n"
            << "options:\n"
            << "  -h, help         show help and exit\n"
            << "  --switch-window  focus previous focused window" << std::endl;
}

std::optional<i3ipc::container_t> i3_find_scratchpad(const cont_list &nodes) {
  for (const auto node : nodes) {
    if (node->type == "workspace" and node->name == "__i3_scratch") {
      return std::make_optional(*node);
    }
  }

  return std::nullopt;
}

std::optional<i3ipc::container_t> App::i3_find_scratchpad() noexcept {
  const auto nodes = i3con->get_tree()->nodes;
  return ::i3_find_scratchpad(nodes);
}

void App::on_server_message(std::string_view message) noexcept {
  if (message != "switch" || !history.size()) {
    return;
  }

  const cont_list nodes = i3con->get_tree()->nodes;
  const auto scratchpad = i3_find_scratchpad();
  const cont_list scratch_nodes =
      scratchpad.has_value() ? scratchpad->nodes : cont_list{};
  const std::function<bool(cont_list, uint64_t)> contains = [&](auto search_in,
                                                                auto id) {
    for (const auto node : search_in) {
      if (node->id == id || contains(node->nodes, id)) {
        return true;
      }
    }
    return false;
  };

  auto it{++history.cbegin()};
  while (it != history.cend()) {
    const uint64_t id = *it;

    if (!contains(scratch_nodes, id) && contains(nodes, id)) {
      i3con->send_command("[con_id=" + std::to_string(id) + "] focus");
      break;
    }

    history.erase(it);
  }
}

void App::on_i3_event(const i3ipc::window_event_t &evt) noexcept {
  if (evt.type != i3ipc::WindowEventType::FOCUS) {
    return;
  }

  uint64_t window_id = evt.container->id;
  const auto found{
      std::find_if(history.cbegin(), history.cend(),
                   [window_id](uint64_t id) { return id == window_id; })};

  if (found != history.cend()) {
    history.erase(found);
  }
  history.insert(history.cbegin(), window_id);

  if (history.size() > App::HISTORY_LEN) {
    history.resize(App::HISTORY_LEN);
  }
}

void App::stop() noexcept { server->stop(); }
