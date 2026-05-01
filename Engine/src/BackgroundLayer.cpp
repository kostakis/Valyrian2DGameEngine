#include "BackgroundLayer.hpp"
#include "MyDebug.hpp"

#include <algorithm>
#include <iostream>

namespace ValyrianEngine {

BackgroundLayer::~BackgroundLayer() {
	// Cleanup should ideally happen via Renderer, but we'd need to store a reference.
	// In Allegro, we can just delete the wrapper if we are sure the handle is destroyed or destroy it here.
	// But let's assume Engine or Renderer handles it for now.
}

void BackgroundLayer::initialize(Renderer& renderer, byte tileW, byte tileH, const std::string& tileBitMapPath,
                                 const std::string& tileCsvFilePath, uint32_t tileBitmapRows1,
                                 std::uint32_t tileBitmapCols1, std::uint32_t screenH, std::uint32_t screenW) {

	Layer::initialize(renderer, tileW, tileH, tileBitMapPath, tileCsvFilePath, tileBitmapRows1, tileBitmapCols1,
	                  screenH, screenW);

	viewWin.width = screenW;
	viewWin.height = screenH;

	if(tilePositions.empty() || tilePositions[0].empty()) {
		return;
	}

	backgroundBitmap = renderer.createBitmap(static_cast<int>(tilePositions[0].size() * 32),
	                                         static_cast<int>(tilePositions.size() * 32));
	assert(backgroundBitmap);

	auto* savedTarget = renderer.saveCurrentTarget();
	renderer.setTargetBitmap(backgroundBitmap);

	for(auto i = 0; i < tilePositions.size(); i++) {
		for(int j = 0; j < tilePositions[i].size(); j++) {
			if(tilePositions[i][j] >= 0) {
				int x = divIndex[tilePositions[i][j]];
				int y = modIndex[tilePositions[i][j]];
				renderer.drawScaledBitmap(tilesBitmap, x * tileWidth, y * tileHeight, tileHeight, tileHeight,
				                          (j * 32),                      // dx
				                          (i * 32), 32, 32, Flip::NONE); // dy
			}
		}
	}

	renderer.restoreTarget(savedTarget);
}

void BackgroundLayer::scroll(float dx) {
	if(!backgroundBitmap)
		return;

	// We need a way to get bitmap info agnostically.
	// Since scroll is usually called from Game, we might need to pass info or have a renderer ref.
	// For now, let's assume it's okay to create a temporary renderer for metadata if needed,
	// but better to have it in the layer.

	Renderer renderer;
	auto max = renderer.getBitmapWidth(backgroundBitmap);
	viewWin.x += dx;
	if(viewWin.x < 0) {
		viewWin.x = max + viewWin.x;
	} else {
		if(viewWin.x >= max) {
			viewWin.x = viewWin.x - max;
		}
	}
}

void BackgroundLayer::render(Renderer& renderer, int xOffset, int yOffset, const ViewData& vw) {
	if(!backgroundBitmap)
		return;
	auto bg_w = renderer.getBitmapWidth(backgroundBitmap);
	auto bg_h = renderer.getBitmapHeight(backgroundBitmap);

	auto w1 = std::min(bg_w - viewWin.x, viewWin.width);
	auto h1 = std::min(bg_h, viewWin.height);
	auto sourceY = std::max((viewWin.height / 32) * 16 + vw.y, 0);

	renderer.drawBitmapRegion(backgroundBitmap,
	                          viewWin.x, // Source bitmap X
	                          sourceY,   // Source bitmap Y
	                          w1,        // Width to copy
	                          h1,        // Height to copy
	                          xOffset, yOffset, Flip::NONE);

	if(w1 < viewWin.width) {          // not whole view win fits
		auto w2 = viewWin.width - w1; // the remaining part
		renderer.drawBitmapRegion(backgroundBitmap, 0, sourceY, w2, h1, w1 + xOffset, yOffset, Flip::NONE);
	}
}

} // namespace ValyrianEngine
