#include "Application.hpp"

int main() {
    Application& app = Application::getInstance();
    app.run();
    return 0;
}
