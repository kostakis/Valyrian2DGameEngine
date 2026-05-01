#ifndef LAYER_HPP
#define LAYER_HPP

#include "Renderer.hpp"
#include "Typedefs.hpp"
#include "ViewData.hpp"

#include <string>
#include <vector>

namespace ValyrianEngine {

class Layer {
public:
	Layer() = default;
	virtual ~Layer() = default;

	virtual void initialize(Renderer& renderer, byte tileW, byte tileH, const std::string& tileBitMapPath,
	                        const std::string& tileCsvFilePath, uint32_t tileBitmapRows1, std::uint32_t tileBitmapCols1,
	                        std::uint32_t screenH, std::uint32_t screenW);

	/*Getters*/
	const std::vector<std::vector<int>>& getTilesVector() const;
	std::uint32_t getRows() const;
	std::uint32_t getColumns() const;
	byte getTileWidth() const;
	byte getTileHeight() const;
	byte getScaledTileWidth() const;
	byte getScaledTileHeight() const;
	Bitmap getTilesBitmap() const;

	/*Setters*/
	void setScalling(byte width, byte height);

	virtual void render(Renderer& renderer, int xOffset = 0, int yOffset = 0, const ViewData& vw = ViewData()) = 0;

protected:
	Bitmap tilesBitmap{nullptr}; // The image containing the tiles

	std::vector<std::vector<int>> tilePositions{};
	std::vector<std::uint32_t> divIndex{};
	std::vector<std::uint32_t> modIndex{};

	byte tileWidth{0};
	byte tileHeight{0};
	byte scaledTileWidth{0};
	byte scaledTileHeight{0};

	std::uint32_t mapRows{0};
	std::uint32_t mapColumns{0};

	std::uint32_t tileBitmapRows{0};
	std::uint32_t tileBitmapColums{0};

	void readTilePositions(const std::string& filePath); // From csv file
};

} // namespace ValyrianEngine
#endif
