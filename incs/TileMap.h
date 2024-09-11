#pragma once

#include <vector>
#include <GL/glew.h>

class TileMap {
public:
    TileMap();

    void Initialize(const unsigned char* data, int imageWidth, int imageHeight);
    bool isInitialized() const;
    int GetTileCount() const;
    int GetTilesPerRow() const;
    GLuint GetTileTextureID(int index) const;

private:
    bool initialized;
    int imageWidth;
    int imageHeight;
    int tilesPerRow;
    std::vector<GLuint> tileTextures;

    void CreateTileTextures(const unsigned char* data);
};
