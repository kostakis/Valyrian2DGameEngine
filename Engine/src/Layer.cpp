#include "Layer.hpp"
#include "BitmapManager.hpp"
#include "MyDebug.hpp"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace ValyrianEngine {

void Layer::initialize(Renderer& renderer, byte tileW, byte tileH, const std::string& tileBitMapPath,
                       const std::string& tileCsvFilePath, uint32_t tileBitmapRows1, std::uint32_t tileBitmapCols1,
                       std::uint32_t screenH, std::uint32_t screenW) {
	tileWidth = tileW;
	tileHeight = tileH;
	tileBitmapRows = tileBitmapRows1;
	tileBitmapColums = tileBitmapCols1;
	tilesBitmap = BitmapManager::getInstance().loadBitmap(tileBitMapPath, tileBitMapPath);
	assert(tilesBitmap);
	readTilePositions(tileCsvFilePath);
}

void Layer::setScalling(byte width, byte height) {
	scaledTileWidth = width;
	scaledTileHeight = height;
}

const std::vector<std::vector<int>>& Layer::getTilesVector() const {
	return tilePositions;
}

std::uint32_t Layer::getRows() const {
	return mapRows;
}

std::uint32_t Layer::getColumns() const {
	return mapColumns;
}

byte Layer::getTileWidth() const {
	return tileWidth;
}

byte Layer::getTileHeight() const {
	return tileHeight;
}

byte Layer::getScaledTileWidth() const {
	return scaledTileWidth;
}

byte Layer::getScaledTileHeight() const {
	return scaledTileHeight;
}

Bitmap Layer::getTilesBitmap() const {
	return tilesBitmap;
}

void Layer::readTilePositions(const std::string& filePath) {
	if(filePath.empty())
		return;
	std::ifstream file(filePath);
	assert(file.is_open());

	std::string line;

	while(std::getline(file, line)) {
		std::vector<int> row;
		std::stringstream ss(line);
		std::string token;

		while(std::getline(ss, token, ',')) {
			if(!token.empty()) {
				try {
					long long val = std::stoll(token);
					// TMX and CSV exports can be complex.
					// We trust the value directly if it's >= 0.
					// This works for both Mario (-1 empty) and Dungeon (0 is floor).
					if(val >= 0) {
						long long gid = val & 0x1FFFFFFF;
						row.push_back(static_cast<int>(gid));
					} else {
						row.push_back(-1); // Empty tile
					}
				} catch(...) {
					// Skip non-numeric
				}
			}
		}
		if(!row.empty()) {
			tilePositions.push_back(row);
		}
	}

	if(tilePositions.empty())
		return;

	mapRows = static_cast<std::uint32_t>(tilePositions.size());
	mapColumns = static_cast<std::uint32_t>(tilePositions[0].size());

	for(std::uint32_t i = 0; i < tileBitmapColums * tileBitmapRows; ++i) {
		divIndex.push_back(i % tileBitmapColums); // X
		modIndex.push_back(i / tileBitmapColums); // Y
	}
}

} // namespace ValyrianEngine
