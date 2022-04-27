#include <csignal>
#include <iostream>

#include "app.hpp"

App app;
bool interrupted;

void stop(int _) {
  interrupted = true;
  app.exit();
}

int main(int argc, char *argv[]) {
  const std::vector<char *> args(argv, argv + argc);

  try {
    app.execute(args);
  } catch (...) {
    if (!interrupted) {
      throw;
    }
  }

  return 0;
}
