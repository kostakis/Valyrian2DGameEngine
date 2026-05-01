#ifndef GRID_HPP
#define GRID_HPP

#define GRID_THIN_AIR_MASK 0x0000     // element is ignored
#define GRID_LEFT_SOLID_MASK 0x0001   // bit 0
#define GRID_RIGHT_SOLID_MASK 0x0002  // bit 1
#define GRID_TOP_SOLID_MASK 0x0004    // bit 2
#define GRID_BOTTOM_SOLID_MASK 0x0008 // bit 3
#define GRID_GROUND_MASK 0x0010       // bit 4, keep objects top / bottom (gravity)
#define GRID_FLOATING_MASK 0x0020     // bit 5, keep objects anywhere inside (gravity)
#define GRID_EMPTY_TILE GRID_THIN_AIR_MASK
#define GRID_SOLID_TILE (GRID_LEFT_SOLID_MASK | GRID_RIGHT_SOLID_MASK | GRID_TOP_SOLID_MASK | GRID_BOTTOM_SOLID_MASK)

typedef unsigned char byte1;

#include "Rectangle.hpp"
#include "TileLayer.hpp"
#include "Typedefs.hpp"

#include <map>
#include <set>
#include <vector>

namespace ValyrianEngine {

struct TileShape {
	float x, y, w, h; // tile-local pixel coords (matching the unscaled tile size)
};
using TileCollisionShapes = std::map<int, std::vector<TileShape>>;

class Grid {
public:
	Grid() = default;
	~Grid() = default;

	void Initialize(const TileLayer* tileLayer);
	void Render(Renderer& renderer, int yOffset = 0, const ViewData& viewData = ViewData(), bool subCells = false,
	            int xOffset = 0);
	void computeGrid();
	void FilterGridMotion(const Rectangle& r, int* dx, int* dy) const;

	/*Setters*/
	inline void setSolidTiles(const std::set<int>& vec) {
		solidTilesIds = vec;
	}

	inline void setTileCollisionShapes(const TileCollisionShapes& shapes) {
		tileShapes = shapes;
	}

	void setTileType(int row, int col, byte1 type);

	bool isOnSolidGround(const Rectangle rec) const;

	std::uint32_t getMapHeight() const {
		return mapHeight;
	}

	void setClampToMapBounds(bool v) {
		m_clampToMapBounds = v;
	}

private:
	bool m_clampToMapBounds{false};
	const TileLayer* tileLayer{nullptr};

	struct GridElement { // Just a small rectangle, cover a small portion of the tile
		byte width{0};
		byte height{0};
	};
	GridElement gridElement{};

	struct GridBlock { // A block has many grid elements, covers all the tile
		byte columns{0};
		byte rows{0};
	};
	GridBlock gridBlock;

	byte elementsPerTile{0};
	std::uint32_t width{};
	std::uint32_t height{};
	std::vector<std::vector<byte>> gridMap;
	std::set<int> solidTilesIds;
	TileCollisionShapes tileShapes;
	std::uint32_t mapWidth{};
	std::uint32_t mapHeight{};

	void renderRectangle(Renderer& renderer, int row, int col, int yOffset, byte1 type, const ViewData& vw) const;
	void rasterizeTileShapes(int row, int col, const std::vector<TileShape>& shapes, float subW, float subH);
	void FilterGridMotionRight(const Rectangle& r, int* dx, int* dy) const;
	void FilterGridMotionLeft(const Rectangle& r, int* dx, int* dy) const;
	void FilterGridMotionUp(const Rectangle& r, int* dx, int* dy) const;
	void FilterGridMotionDown(const Rectangle& r, int* dx, int* dy) const;
	bool canPassTile(int row, int col, byte1 flags) const;
};

} // namespace ValyrianEngine
#endif
