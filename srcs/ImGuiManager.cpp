#include "ImGuiManager.h"
#include "png_loader.h"

constexpr int TILE_SIZE = 32;
constexpr int MAX_LAYERS = 5;
int GRID_WIDTH = 10;
int GRID_HEIGHT = 10;

int selectedTileIndex = -1;
int selectedLayer = 0;

std::vector<std::vector<std::vector<int>>> gridLayers(GRID_WIDTH, std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(MAX_LAYERS, -1)));

GLuint eraseGridTextureID = 0;

void ImGuiManager::LoadEraseGridImage() {
    const std::string eraseGridImagePath = "assets/ui/erase.png";

    unsigned int width, height;
    if (const unsigned char* data = load_png(eraseGridImagePath, &width, &height)) {
        glGenTextures(1, &eraseGridTextureID);
        glBindTexture(GL_TEXTURE_2D, eraseGridTextureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        free_image_data(data);
    } else {
        std::cout << "Failed to load erase grid image: " << eraseGridImagePath << std::endl;
    }
}


void ImGuiManager::Initialize(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 150");

    ImGui::GetIO().IniFilename = nullptr;

    gridLayers.resize(GRID_WIDTH, std::vector<std::vector<int>>(GRID_HEIGHT, std::vector<int>(MAX_LAYERS, -1)));

    LoadEraseGridImage();
}

void ImGuiManager::LoadImage() {
    const std::string imagePath = "assets/1.png";

    unsigned int width, height;

    if (const unsigned char* data = load_png(imagePath, &width, &height)) {
        tileMap.Initialize(data, width, height);
        free_image_data(data);
    } else {
        std::cout << "Failed to load image: " << imagePath << std::endl;
    }
}

void ImGuiManager::DrawMenu() {
    ImGui::SetNextWindowPos(ImVec2(300, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 100));
    ImGui::Begin("Map Editor");

    static int newWidth = GRID_WIDTH;
    static int newHeight = GRID_HEIGHT;

    if (ImGui::SliderInt("Grid Width", &newWidth, 1, 100) || ImGui::SliderInt("Grid Height", &newHeight, 1, 100)) {
        ResizeGrid(newWidth, newHeight);
    }

    if (ImGui::Button("Create Grid")) {
        createGrid = true;
    }

    ImGui::End();
}


void ImGuiManager::Render(GLFWwindow* window) {
    constexpr auto clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    LoadImage();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (tileMap.isInitialized()) {
            ImGui::SetNextWindowPos(ImVec2(1050, 0));
            ImGui::Begin("Tile Selector");

            for (int i = 0; i < tileMap.GetTileCount(); ++i) {
                if (ImGui::ImageButton(std::to_string(i).c_str(), reinterpret_cast<ImTextureID>(static_cast<intptr_t>(tileMap.GetTileTextureID(i))), ImVec2(TILE_SIZE, TILE_SIZE))) {
                    selectedTileIndex = i;
                }

                if ((i + 1) % tileMap.GetTilesPerRow() != 0) {
                    ImGui::SameLine();
                }
            }

            ImGui::End();
        }

        DrawMenu();
        DrawToolsMenu();
        DrawSaveMenu();

        if (createGrid) {
            ImGui::SetNextWindowPos(ImVec2(0, 100));
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            DrawGrid(drawList);

            if (ImGui::IsMouseClicked(0)) {
                const ImVec2 mousePos = ImGui::GetMousePos();
                HandleGridClick(mousePos.x, mousePos.y);
            }
        }


        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
}

void ImGuiManager::DrawGrid(ImDrawList* drawList) const {
    const ImVec2 window_pos = ImGui::GetWindowPos();
    const ImVec2 window_size = ImGui::GetWindowSize();

    const float toolbarHeight = ImGui::GetCursorPosY();

    const float gridSizeX = window_size.x / GRID_WIDTH;
    const float gridSizeY = window_size.y / GRID_HEIGHT;

    for (int x = 0; x < GRID_WIDTH; ++x) {
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            const float startX = window_pos.x + x * gridSizeX;
            const float startY = window_pos.y + toolbarHeight + y * gridSizeY;

            drawList->AddLine(ImVec2(startX, startY), ImVec2(startX, startY + gridSizeY), IM_COL32(255, 255, 255, 255));
            drawList->AddLine(ImVec2(startX, startY), ImVec2(startX + gridSizeX, startY), IM_COL32(255, 255, 255, 255));

            for (int layer = 0; layer < MAX_LAYERS; ++layer) {
                if (gridLayers[x][y][layer] != -1) {
                    const GLuint tileTextureID = tileMap.GetTileTextureID(gridLayers[x][y][layer]);
                    drawList->AddImage(reinterpret_cast<void *>(static_cast<intptr_t>(tileTextureID)),
                        ImVec2(startX, startY),
                        ImVec2(startX + gridSizeX, startY + gridSizeY));
                }
            }
        }
    }
}

void ImGuiManager::HandleGridClick(const float mouseX, const float mouseY) {
    if (selectedTileIndex == -1) return;

    const ImVec2 window_pos = ImGui::GetWindowPos();
    const ImVec2 window_size = ImGui::GetWindowSize();

    const float toolbarHeight = ImGui::GetCursorPosY();

    const float relativeX = mouseX - window_pos.x;
    const float relativeY = mouseY - window_pos.y - toolbarHeight;

    const float gridSizeX = window_size.x / GRID_WIDTH;
    const float gridSizeY = window_size.y / GRID_HEIGHT;

    const int gridX = static_cast<int>(relativeX / gridSizeX);
    const int gridY = static_cast<int>(relativeY / gridSizeY);

    if (gridX >= 0 && gridX < GRID_WIDTH  && gridY >= 0 && gridY < GRID_HEIGHT) {
        gridLayers[gridX][gridY][selectedLayer] = selectedTileIndex;
    }
}

void ImGuiManager::ResizeGrid(const int newWidth, const int newHeight) {
    std::vector<std::vector<std::vector<int>>> newGridLayers(newWidth, std::vector<std::vector<int>>(newHeight, std::vector<int>(MAX_LAYERS, -1)));

    for (int x = 0; x < std::min(GRID_WIDTH, newWidth); ++x) {
        for (int y = 0; y < std::min(GRID_HEIGHT, newHeight); ++y) {
            for (int layer = 0; layer < MAX_LAYERS; ++layer) {
                newGridLayers[x][y][layer] = gridLayers[x][y][layer];
            }
        }
    }
    gridLayers = newGridLayers;
    GRID_WIDTH = newWidth;
    GRID_HEIGHT = newHeight;
}

void ImGuiManager::SaveMap(const std::string& filename) const {
    if (std::ofstream file(filename); file.is_open()) {
        file << "[\n";
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            file << " \t[";
            for (int x = 0; x < GRID_WIDTH; ++x) {
                file << "[";
                for (int layer = 0; layer < MAX_LAYERS; ++layer) {
                    file << gridLayers[x][y][layer];
                    if (layer < MAX_LAYERS - 1) {
                        file << ", ";
                    }
                }
                file << "]";
                if (x < GRID_WIDTH - 1) {
                    file << ", ";
                }
            }
            file << "]";
            if (y < GRID_HEIGHT - 1) {
                file << ",\n";
            }
        }
        file << "\n]";
        file.close();
        std::cout << "Map saved to " << filename << std::endl;
    } else {
        std::cout << "Failed to open file for saving!" << std::endl;
    }
}


void ImGuiManager::DrawSaveMenu() const {
    ImGui::SetNextWindowPos(ImVec2(600, 0));
    ImGui::SetNextWindowSize(ImVec2(200, 100));
    ImGui::Begin("Save Menu");

    if (ImGui::Button("Save Map")) {
        SaveMap("saved_map.json");
    }

    ImGui::End();
}

void ImGuiManager::DrawToolsMenu() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(300, 100));
    ImGui::Begin("Tools");

    if (selectedTileIndex != -1) {
        if (ImGui::Button("Paint all")) {
            for (auto& layer : gridLayers) {
                for (auto& row : layer) {
                    row[selectedLayer] = selectedTileIndex;
                }
            }
        }
    }

    if (eraseGridTextureID != 0) {
        if (ImGui::ImageButton("EraseGridButton", reinterpret_cast<ImTextureID>(eraseGridTextureID), ImVec2(TILE_SIZE, TILE_SIZE))) {
            for (auto& layer : gridLayers) {
                for (auto& row : layer) {
                    row[selectedLayer] = -1;
                }
            }
        }
    }
    ImGui::SliderInt("Select Layer", &selectedLayer, 0, MAX_LAYERS - 1);
    ImGui::End();
}

void ImGuiManager::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
