#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "BackgroundLayer.hpp"
#include "Logger.hpp"
#include "RandomGenerator.hpp"
#include "Renderer.hpp"
#include "TileLayer.hpp"
#include "Typedefs.hpp"
#include "ViewData.hpp"

#include <map>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace ValyrianEngine {

class Engine {
public:
	static constexpr uint16_t s_windowWidth{1280};
	static constexpr uint16_t s_windowHeight{736};

	Engine();
	~Engine();

	void initialize();
	void createWindow();

	// Layers
	void initializeTileLayer(byte tileWidth, byte tileHeight, const std::string& tileBitMapPath,
	                         const std::string& tileCsvFilePath, std::uint32_t tileBitmapRows1,
	                         std::uint32_t tileBitmapCols1);
	void initializeBackgroundLayer(byte tileWidth, byte tileHeight, const std::string& tileBitMapPath,
	                               const std::string& tileCsvFilePath, std::uint32_t tileBitmapRows1,
	                               std::uint32_t tileBitmapCols1);
	void resetLayers();

	Bitmap createBitmap(std::uint32_t width, std::uint32_t height);
	void destroyBitmap(Bitmap b);
	void setTargetBitmap(Bitmap b);
	void clearBitmap(Bitmap b, Color c = Color::RGB(0, 0, 0));
	void clearCurrentBitmap(Color c = Color::RGB(0, 0, 0));
	void drawBitmap(Bitmap b, int x, int y);
	void clearDisplay();
	void updateDisplay();
	void makeDisplayCurrentBitmap();
	Event* processInput();
	void loadFont(const std::string& id, const std::string& fontFullPath, int size);

	/*Colors*/
	Color color(byte red, byte green, byte blue);

	/*Getters*/
	TileLayer* getTileLayer() const;
	BackgroundLayer* getBackgroundLayer() const;
	Display getDisplay() const;
	ViewData& getViewData();
	std::uint32_t getMapWidth() const;
	std::uint32_t getMapHeight() const;
	EventQueue getEventQueue() const;
	Timer getFPSTimer() const;
	Timer getLogicTimer() const;
	double getRenderFps() const;
	Font getFont(const std::string& id) const;
	Renderer& getRenderer() {
		return m_renderer;
	}

	RandomGenerator& getRandomGenerator();

	/*Misc*/
	// Draw with the default fonts and colors
	void drawText(const char* text, int x, int y, Font font = nullptr, Color color = Color::RGB(255, 255, 255));

private:
	std::shared_ptr<spdlog::logger> m_logger;

	// The Renderer
	Renderer m_renderer;

	// The actual display window
	Display display{nullptr};

	// Layers
	std::unique_ptr<TileLayer> tileLayer;
	std::unique_ptr<BackgroundLayer> backgroundLayer;

	// RNG - Random generator
	RandomGenerator m_randomGenerator{};

	ViewData viewData{};

	// Fonts
	Font m_defaultFont{nullptr};
	std::map<std::string, Font> m_fontMap;

	// Map dimensions drawed to the display bitmap
	std::uint32_t mapWidth{0};
	std::uint32_t mapHeight{0};

	// Input processing
	EventQueue eventQueue{nullptr};
	Event ev{};

	// Timers
	Timer FPSTimer{nullptr};
	Timer logicTimer{nullptr}; // Keyboard events

	// Music - TODO Can be done better ..
	// We'll keep these as pointers for now, but they are NOT agnostic yet.
	// Since we are focused on rendering, we'll fix audio later or wrap it.
	void* m_voice{nullptr};
	void* m_mixer{nullptr};

	static constexpr double FPS{60};
	static constexpr double logicFPS{60};

	/*Private Methods*/
	void initBackend();
};

} // namespace ValyrianEngine

#endif
