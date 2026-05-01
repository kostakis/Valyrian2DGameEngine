#ifndef TILELAYER_HPP
#define TILELAYER_HPP

#include "Layer.hpp"

#include <set>
#include <vector>

namespace ValyrianEngine {

class TileLayer : public Layer {
public:
	TileLayer() = default;
	~TileLayer() = default;

	void render(Renderer& renderer, int xOffset = 0, int yOffset = 0, const ViewData& vw = ViewData()) override;
	bool checkMiddlePosition(int x, const ViewData& vw) const;
	void setInvisibleTiles(const std::set<uint16_t>&);
	void setInvisibleTiles(int x, int y);

	std::vector<std::pair<int, int>> getTilesWithId(int);

private:
	std::set<uint16_t> m_invisibleTilesIdSet;
};

} // namespace ValyrianEngine
#endif
