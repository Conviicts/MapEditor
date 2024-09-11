#pragma once

#include "TileMap.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <map>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

class ImGuiManager {
public:
    std::string imagePath;
    TileMap tileMap;
    bool createGrid = false;

    int selectedTileIndex = -1;
    std::vector<int> gridTiles;

    void Initialize(GLFWwindow* window);
    void Render(GLFWwindow* window);
    static void Shutdown();
    void LoadImage();
    void DrawGrid(ImDrawList *drawList) const;

    void DrawToolsMenu();
    void DrawMenu();
    void DrawSaveMenu() const;
    void HandleGridClick(float mouseX, float mouseY);
    void ResizeGrid(int newWidth, int newHeight);
    void SaveMap(const std::string& filename) const;

    static void LoadEraseGridImage();
private:
    std::vector<std::vector<std::vector<int>>> gridLayers;
};

