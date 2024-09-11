#pragma once

#include "WindowManager.h"
#include "ImGuiManager.h"

class Application {
public:
    WindowManager windowManager;
    ImGuiManager imguiManager;

    int Run();
};
