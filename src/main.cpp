#include "application/Application.hpp"
int main() {
  auto *app = new Application();
  app->run();
  delete app;
  return 0;
}