#ifndef BACKGROUNDLAYER_HPP
#define BACKGROUNDLAYER_HPP

#include "Layer.hpp"

namespace ValyrianEngine {

class BackgroundLayer : public Layer {
public:
	typedef struct myRec {
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	} LayerRectangle; // Renamed to avoid collision with ValyrianEngine::Rectangle

	BackgroundLayer() = default;
	~BackgroundLayer();

	void initialize(Renderer& renderer, byte tileW, byte tileH, const std::string& tileBitMapPath,
	                const std::string& tileCsvFilePath, uint32_t tileBitmapRows1, std::uint32_t tileBitmapCols1,
	                std::uint32_t screenH, std::uint32_t screenW) override;

	virtual void render(Renderer& renderer, int xOffset = 0, int yOffset = 0, const ViewData& vw = ViewData()) override;

	void scroll(float dx);

private:
	Bitmap backgroundBitmap{nullptr};

	std::uint32_t backgroundWidth{0};
	std::uint32_t backgroundHeight{0};

	LayerRectangle viewWin;
};

} // namespace ValyrianEngine
#endif
