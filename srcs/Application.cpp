#include "Application.h"

int Application::Run() {
    if (windowManager.Initialize() != 0) {
        return -1;
    }

    imguiManager.Initialize(windowManager.window);
    imguiManager.Render(windowManager.window);
    ImGuiManager::Shutdown();

    return 0;
}
