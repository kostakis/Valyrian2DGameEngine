#include "Grid.hpp"
#include "MyDebug.hpp"
#include "Renderer.hpp"

#include <algorithm>
#include <iostream>

#define DIV_GRID_ELEMENT_WIDTH(i) ((i) >> 3)
#define DIV_GRID_ELEMENT_HEIGHT(i) ((i) >> 3)
#define MUL_GRID_ELEMENT_WIDTH(i) ((i) << 3)
#define MUL_GRID_ELEMENT_HEIGHT(i) ((i) << 3)

namespace ValyrianEngine {

void Grid::Initialize(const TileLayer* tileLayer1) {
	tileLayer = tileLayer1;

	gridElement.width = tileLayer->getScaledTileWidth() / 4;   // 32 / 4 = 8
	gridElement.height = tileLayer->getScaledTileHeight() / 4; // 32 / 4 = 8

	gridBlock.columns = tileLayer->getScaledTileWidth() / gridElement.width; // = 4
	gridBlock.rows = tileLayer->getScaledTileHeight() / gridElement.height;  // = 4

	elementsPerTile = gridBlock.columns * gridBlock.rows; // = 16

	mapWidth = tileLayer->getColumns() * gridBlock.columns;
	mapHeight = tileLayer->getRows() * gridBlock.rows;

	gridMap.assign(mapHeight, std::vector<byte>(mapWidth));
}

void Grid::computeGrid() {
	auto& vecMap = tileLayer->getTilesVector();
	float tileLocalW = static_cast<float>(tileLayer->getTileWidth());
	float tileLocalH = static_cast<float>(tileLayer->getTileHeight());
	float subLocalW = tileLocalW / gridBlock.columns;
	float subLocalH = tileLocalH / gridBlock.rows;

	for(int i = 0; i < vecMap.size(); i++) {
		for(int j = 0; j < vecMap[i].size(); j++) {
			int tileId = vecMap[i][j];

			auto shapeIt = tileShapes.find(tileId);
			if(shapeIt != tileShapes.end()) {
				rasterizeTileShapes(i, j, shapeIt->second, subLocalW, subLocalH);
			} else if(solidTilesIds.find(tileId) != solidTilesIds.end()) {
				setTileType(i, j, GRID_SOLID_TILE);
			} else {
				setTileType(i, j, GRID_EMPTY_TILE);
			}
		}
	}
}

void Grid::rasterizeTileShapes(int row, int col, const std::vector<TileShape>& shapes, float subW, float subH) {
	auto startRow = row * gridBlock.rows;
	auto startCol = col * gridBlock.columns;

	for(int sr = 0; sr < gridBlock.rows; sr++) {
		for(int sc = 0; sc < gridBlock.columns; sc++) {
			float cellX = sc * subW;
			float cellY = sr * subH;
			float cellX2 = cellX + subW;
			float cellY2 = cellY + subH;

			byte1 mask = GRID_EMPTY_TILE;
			for(const auto& s : shapes) {
				float sx2 = s.x + s.w;
				float sy2 = s.y + s.h;
				if(cellX < sx2 && cellX2 > s.x && cellY < sy2 && cellY2 > s.y) {
					mask = GRID_SOLID_TILE;
					break;
				}
			}

			int gr = startRow + sr;
			int gc = startCol + sc;
			if(gr < (int)gridMap.size() && gc < (int)gridMap[gr].size()) {
				gridMap[gr][gc] = mask;
			}
		}
	}
}

void Grid::Render(Renderer& renderer, int yOffset, const ViewData& vw, bool subCells, int xOffset) {
	auto& vec = tileLayer->getTilesVector();
	if(vec.empty() || vec[0].empty())
		return;

	int startCol = DIV_TILE_WIDTH(vw.x);
	int endCol = DIV_TILE_WIDTH(vw.x + vw.windowWidth) + 1;

	startCol = std::max(0, startCol);
	endCol = std::min((int)vec[0].size(), endCol);

	auto tileWidth = tileLayer->getScaledTileWidth();
	auto tileHeight = tileLayer->getScaledTileHeight();

	const Color solidColor = Color::RGB(255, 0, 0);
	const Color emptyColor = Color::RGB(0, 255, 0);

	if(!subCells) {
		for(auto i = 0; i < (int)vec.size(); i++) {
			for(int j = startCol; j < endCol; j++) {
				if(vec[i][j] < 0)
					continue;

				auto gridRow = i * gridBlock.rows;
				auto gridCol = j * gridBlock.columns;

				if(gridRow >= (int)gridMap.size() || gridCol >= (int)gridMap[gridRow].size())
					continue;

				auto color = (gridMap[gridRow][gridCol] & GRID_SOLID_TILE) ? solidColor : emptyColor;
				renderer.drawRectangle((j * tileWidth) + xOffset - vw.x, (i * tileHeight) + yOffset - vw.y,
				                       (j * tileWidth) + tileWidth + xOffset - vw.x,
				                       (i * tileHeight) + tileHeight + yOffset - vw.y, color, 2);
			}
		}
		return;
	}

	auto subW = gridElement.width;
	auto subH = gridElement.height;

	int startRow = (vw.y - yOffset) / tileHeight;
	int endRow = ((vw.y - yOffset) + vw.windowHeight) / tileHeight + 1;
	startRow = std::max(0, startRow);
	endRow = std::min((int)vec.size(), endRow);

	for(int i = startRow; i < endRow; i++) {
		for(int j = startCol; j < endCol; j++) {
			if(vec[i][j] < 0)
				continue;

			auto gridRowBase = i * gridBlock.rows;
			auto gridColBase = j * gridBlock.columns;

			if(gridRowBase >= (int)gridMap.size() || gridColBase >= (int)gridMap[gridRowBase].size())
				continue;

			byte1 first = gridMap[gridRowBase][gridColBase];
			bool uniform = true;
			for(int sr = 0; sr < gridBlock.rows && uniform; sr++) {
				for(int sc = 0; sc < gridBlock.columns; sc++) {
					if(gridMap[gridRowBase + sr][gridColBase + sc] != first) {
						uniform = false;
						break;
					}
				}
			}

			if(uniform) {
				if(!(first & GRID_SOLID_TILE))
					continue;
				renderer.drawRectangle((j * tileWidth) + xOffset - vw.x, (i * tileHeight) + yOffset - vw.y,
				                       (j * tileWidth) + tileWidth + xOffset - vw.x,
				                       (i * tileHeight) + tileHeight + yOffset - vw.y, solidColor, 2);
				continue;
			}

			for(int sr = 0; sr < gridBlock.rows; sr++) {
				for(int sc = 0; sc < gridBlock.columns; sc++) {
					auto cell = gridMap[gridRowBase + sr][gridColBase + sc];
					if(!(cell & GRID_SOLID_TILE))
						continue;

					float x1 = (j * tileWidth) + sc * subW + xOffset - vw.x;
					float y1 = (i * tileHeight) + sr * subH + yOffset - vw.y;
					renderer.drawRectangle(x1, y1, x1 + subW, y1 + subH, solidColor, 1);
				}
			}
		}
	}
}

void Grid::setTileType(int row, int col, byte1 type) {
	auto startRow = row * gridBlock.rows;
	auto endRow = startRow + gridBlock.rows;

	auto startCol = col * gridBlock.columns;
	auto endCol = startCol + gridBlock.columns;

	for(int i = startRow; i < endRow; i++) {
		for(int j = startCol; j < endCol; j++) {
			if(i < gridMap.size() && j < gridMap[i].size()) {
				gridMap[i][j] = type;
			}
		}
	}
}

bool Grid::isOnSolidGround(const Rectangle rec) const {
	int dy = 1;
	int dx = 0;
	FilterGridMotion(rec, &dx, &dy);

	if(dy == 0)
		return true;
	else
		return false;
}

void Grid::renderRectangle(Renderer& renderer, int row, int col, int yOffset, byte1 type, const ViewData& vw) const {
	auto tileWidth = tileLayer->getScaledTileWidth();
	auto tileHeight = tileLayer->getScaledTileHeight();

	const Color color = (type & GRID_SOLID_TILE) ? Color::RGB(255, 0, 0) : Color::RGB(0, 255, 0);

	for(int k = 0; k < elementsPerTile; k += gridBlock.columns) {
		for(int q = 0; q < elementsPerTile; q += gridBlock.rows) {
			renderer.drawRectangle((col * tileWidth) + k - vw.x, (row * tileHeight) + yOffset + q,
			                       (col * tileWidth) + k + 4 - vw.x, (row * tileHeight) + q + 4 + yOffset, color, 1);
		}
	}
}

void Grid::FilterGridMotion(const Rectangle& r, int* dx, int* dy) const {
	// Boundary check for dx/dy - clamping to grid element size is expected by logic
	// but we should ensure we don't crash if they are slightly larger

	if(*dx > 0)
		FilterGridMotionRight(r, dx, dy);
	else if(*dx < 0)
		FilterGridMotionLeft(r, dx, dy);

	if(*dy > 0)
		FilterGridMotionDown(r, dx, dy);
	else if(*dy < 0)
		FilterGridMotionUp(r, dx, dy);
}

void Grid::FilterGridMotionRight(const Rectangle& r, int* dx, int* dy) const {
	auto x2 = r.x + r.screenWidth - 1;
	auto x2Next = x2 + *dx;

	if(m_clampToMapBounds) {
		auto mapPixelWidth = MUL_GRID_ELEMENT_WIDTH(mapWidth);
		if(x2Next >= mapPixelWidth) {
			*dx = mapPixelWidth - 1 - x2;
			return;
		}
	}

	auto currCol = DIV_GRID_ELEMENT_WIDTH(r.x);
	auto newCol = DIV_GRID_ELEMENT_WIDTH(x2Next);

	if(currCol != newCol) {
		auto startRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
		auto endRow = DIV_GRID_ELEMENT_HEIGHT(r.y + r.screenHeight - 1);

		for(int row = startRow; row <= endRow; ++row) {
			if(canPassTile(row, newCol, GRID_LEFT_SOLID_MASK)) {
				*dx = MUL_GRID_ELEMENT_WIDTH(newCol) - (x2 + 1);
				break;
			}
		}
	}
}

void Grid::FilterGridMotionLeft(const Rectangle& r, int* dx, int* dy) const {
	auto x2Next = r.x + *dx;

	if(x2Next < 0) {
		*dx = -r.x;
		return;
	}

	auto currCol = DIV_GRID_ELEMENT_WIDTH(r.x);
	auto newCol = DIV_GRID_ELEMENT_WIDTH(x2Next);
	if(newCol != currCol) {
		auto startRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
		auto endRow = DIV_GRID_ELEMENT_HEIGHT(r.y + r.screenHeight - 1);

		for(int i = startRow; i <= endRow; ++i) {
			if(canPassTile(i, newCol, GRID_RIGHT_SOLID_MASK)) {
				*dx = MUL_GRID_ELEMENT_WIDTH(currCol) - r.x;
				break;
			}
		}
	}
}

void Grid::FilterGridMotionUp(const Rectangle& r, int* dx, int* dy) const {
	auto y2Next = r.y + *dy;

	if(y2Next < 0) {
		*dy = -r.y;
		return;
	}

	auto currRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
	auto nextRow = DIV_GRID_ELEMENT_HEIGHT(y2Next);
	if(currRow != nextRow) {
		auto startCol = DIV_GRID_ELEMENT_WIDTH(r.x);
		auto endCol = DIV_GRID_ELEMENT_WIDTH(r.x + r.screenWidth - 1);

		for(int col = startCol; col <= endCol; col++) {
			if(canPassTile(nextRow, col, GRID_BOTTOM_SOLID_MASK)) {
				*dy = MUL_GRID_ELEMENT_HEIGHT(currRow) - r.y;
				break;
			}
		}
	}
}

void Grid::FilterGridMotionDown(const Rectangle& r, int* dx, int* dy) const {
	auto y2 = r.y + r.screenHeight - 1;
	auto y2Next = y2 + *dy;

	if(m_clampToMapBounds) {
		auto mapPixelHeight = MUL_GRID_ELEMENT_HEIGHT(mapHeight);
		if(y2Next >= mapPixelHeight) {
			*dy = mapPixelHeight - 1 - y2;
			return;
		}
	}

	auto currRow = DIV_GRID_ELEMENT_HEIGHT(r.y);
	auto nextRow = DIV_GRID_ELEMENT_HEIGHT(y2Next);

	if(currRow != nextRow) {
		auto startCol = DIV_GRID_ELEMENT_WIDTH(r.x);
		auto endCol = DIV_GRID_ELEMENT_WIDTH(r.x + r.screenWidth - 1);

		for(int col = startCol; col <= endCol; col++) {
			if(canPassTile(nextRow, col, GRID_TOP_SOLID_MASK)) {
				*dy = MUL_GRID_ELEMENT_HEIGHT(nextRow) - (y2 + 1);
				break;
			}
		}
	}
}

bool Grid::canPassTile(int row, int col, byte1 flags) const {
	if(row >= 0 && row < (int)gridMap.size() && col >= 0 && col < (int)gridMap[row].size()) {
		return (gridMap[row][col] & flags) != 0;
	}
	return false; // Out of bounds = empty (passable). Caller is responsible for off-map cleanup.
}

} // namespace ValyrianEngine
