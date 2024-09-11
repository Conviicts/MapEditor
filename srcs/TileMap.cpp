#include "TileMap.h"
#include <GL/gl.h>
#include <iostream>

TileMap::TileMap() : initialized(false), imageWidth(0), imageHeight(0), tilesPerRow(0) {}

void TileMap::Initialize(const unsigned char* data, int width, int height) {
    imageWidth = width;
    imageHeight = height;
    tilesPerRow = width / 32;

    CreateTileTextures(data);
    initialized = true;
}

bool TileMap::isInitialized() const {
    return initialized;
}

int TileMap::GetTileCount() const {
    return (imageWidth / 32) * (imageHeight / 32);
}

int TileMap::GetTilesPerRow() const {
    return tilesPerRow;
}

GLuint TileMap::GetTileTextureID(int index) const {
    if (index >= 0 && index < tileTextures.size()) {
        return tileTextures[index];
    }
    return 0;
}

void TileMap::CreateTileTextures(const unsigned char* data) {
    constexpr int tileHeight = 32;

    for (int y = 0; y < imageHeight; y += tileHeight) {
        constexpr int tileWidth = 32;
        for (int x = 0; x < imageWidth; x += tileWidth) {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            unsigned char tileData[32 * 32 * 4];

            for (int row = 0; row < tileHeight; ++row) {
                for (int col = 0; col < tileWidth; ++col) {
                    const int srcIndex = ((y + row) * imageWidth + (x + col)) * 4;
                    const int dstIndex = (row * tileWidth + col) * 4;

                    tileData[dstIndex] = data[srcIndex];         // R
                    tileData[dstIndex + 1] = data[srcIndex + 1]; // G
                    tileData[dstIndex + 2] = data[srcIndex + 2]; // B
                    tileData[dstIndex + 3] = data[srcIndex + 3]; // A
                }
            }

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tileWidth, tileHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, tileData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            tileTextures.push_back(textureID);
        }
    }
}
