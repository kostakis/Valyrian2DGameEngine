#include "TileLayer.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace ValyrianEngine {

void TileLayer::render(Renderer& renderer, int xOffset, int yOffset, const ViewData& vw) {
	if(tilePositions.empty() || tilePositions[0].empty())
		return;

	int startCol = DIV_TILE_WIDTH(vw.x); // vw.x / scaledTileWidth;
	int endCol = DIV_TILE_WIDTH(vw.x + vw.windowWidth) + 1 /*Too catch up screen*/;

	startCol = std::max(0, startCol);
	endCol = std::min((int)tilePositions[0].size(), endCol);

	if(startCol >= endCol)
		return;

	renderer.holdDrawing(true);
	for(auto i = 0; i < tilePositions.size(); ++i) {
		for(auto j = startCol; j < endCol; ++j) {
			if(tilePositions[i][j] >= 0) {
				auto x = divIndex[tilePositions[i][j]];
				auto y = modIndex[tilePositions[i][j]];
				renderer.drawScaledBitmap(tilesBitmap, x * tileWidth, y * tileHeight, tileWidth, tileHeight,
				                          (j * scaledTileWidth) + xOffset - vw.x,
				                          (i * scaledTileHeight) + yOffset - vw.y, scaledTileWidth, scaledTileHeight,
				                          Flip::NONE);
			}
		}
	}
	renderer.holdDrawing(false);
}

bool TileLayer::checkMiddlePosition(int x, const ViewData& vw) const {
	auto width = scaledTileWidth;

	auto startCol = DIV_TILE_WIDTH(vw.x);
	auto endCol =
	    ((vw.x + vw.windowWidth) / width); // DIV_TILE_WIDTH(vw.x + vw.windowWidth) + 1/*Too catch up screen*/;

	auto xCol = x / width;

	if(xCol > ((startCol + endCol) / 2))
		return true;
	else
		return false;
}

void TileLayer::setInvisibleTiles(const std::set<uint16_t>& ids) {
	m_invisibleTilesIdSet = ids;
}

void TileLayer::setInvisibleTiles(int x, int y) {
	if(x >= 0 && x < tilePositions.size() && y >= 0 && y < tilePositions[x].size()) {
		tilePositions[x][y] = -1;
	}
}

std::vector<std::pair<int, int>> TileLayer::getTilesWithId(int id) {
	std::vector<std::pair<int, int>> vec;

	for(auto i = 0; i < tilePositions.size(); ++i) {
		for(auto j = 0; j < tilePositions[i].size(); ++j) {
			auto tileId = tilePositions[i][j];
			if(tileId == id) {
				vec.push_back(std::make_pair<int, int>(j * scaledTileWidth, i * scaledTileHeight));
			}
		}
	}
	return vec;
}

} // namespace ValyrianEngine
