#include "Application.h"

int main() {
    Application& app = Application::getInstance();
    app.run();
    return 0;
}
