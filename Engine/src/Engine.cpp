#include "Engine.hpp"
#include "BitmapManager.hpp"
#include "MyDebug.hpp"

namespace ValyrianEngine {

void Engine::initBackend() {
	m_renderer.initSystem();
	m_renderer.initAudio(&m_voice, &m_mixer);
}

Engine::~Engine() {
	m_logger->trace("Engine destructor");

	m_renderer.destroyDisplay(display);
	m_renderer.destroyTimer(FPSTimer);
	m_renderer.destroyTimer(logicTimer);
	m_renderer.destroyEventQueue(eventQueue);

	if(m_defaultFont)
		m_renderer.destroyFont(m_defaultFont);
	for(auto& x : m_fontMap)
		m_renderer.destroyFont(x.second);

	m_renderer.destroyAudio(m_voice, m_mixer);
}

Engine::Engine() {
	m_logger = getOrCreateLogger("Engine");
	m_logger->trace("Engine constructor");
}

void Engine::initialize() {
	initBackend();
	BitmapManager::getInstance().initialize(&m_renderer);
	m_defaultFont = m_renderer.loadFont("media/fonts/CrimsonText-Regular.ttf", 16, 0);
}

void Engine::createWindow() {
	bool fullScreen{false};
	if(fullScreen) {
		int monW{}, monH{};
		m_renderer.getMonitorSize(0, &monW, &monH);
		viewData.windowWidth = static_cast<uint16_t>(monW);
		viewData.windowHeight = static_cast<uint16_t>(monH);
	} else {
		viewData.windowWidth = Engine::s_windowWidth;
		viewData.windowHeight = Engine::s_windowHeight;
	}

	display = m_renderer.createDisplay(viewData.windowWidth, viewData.windowHeight, fullScreen);
	assert(display);

	m_renderer.setDisplayIcon(display, BitmapManager::getInstance().loadBitmap("icon", "media/appicon.png"));

	eventQueue = m_renderer.createEventQueue();
	assert(eventQueue);

	FPSTimer = m_renderer.createTimer(FPS);
	assert(FPSTimer);

	logicTimer = m_renderer.createTimer(logicFPS);
	assert(logicTimer);

	m_renderer.startTimer(FPSTimer);
	m_renderer.startTimer(logicTimer);

	m_renderer.registerEventSources(eventQueue, display, FPSTimer, logicTimer);

	m_renderer.makeDisplayCurrentBitmap(display);
}

void Engine::initializeTileLayer(byte tileWidth, byte tileHeight, const std::string& tileBitMapPath,
                                 const std::string& tileCsvFilePath, std::uint32_t tileBitmapRows1,
                                 std::uint32_t tileBitmapCols1) {
	tileLayer = std::make_unique<TileLayer>();
	tileLayer->initialize(m_renderer, tileWidth, tileHeight, tileBitMapPath, tileCsvFilePath, tileBitmapRows1,
	                      tileBitmapCols1, viewData.windowHeight, viewData.windowWidth);
	mapWidth = tileLayer->getColumns() * tileWidth;
	mapHeight = tileLayer->getRows() * tileHeight;
	if(mapHeight * 2 > viewData.windowHeight) {
		m_logger->warn("Map height is smaller than view window not full terrain visible");
		m_logger->warn("Terrain height: {}", mapHeight * 2);
		m_logger->warn("Window height: {}", viewData.windowHeight);
	}
}

void Engine::initializeBackgroundLayer(byte tileWidth, byte tileHeight, const std::string& tileBitMapPath,
                                       const std::string& tileCsvFilePath, std::uint32_t tileBitmapRows1,
                                       std::uint32_t tileBitmapCols1) {
	backgroundLayer = std::make_unique<BackgroundLayer>();
	backgroundLayer->initialize(m_renderer, tileWidth, tileHeight, tileBitMapPath, tileCsvFilePath, tileBitmapRows1,
	                            tileBitmapCols1, viewData.windowHeight, viewData.windowWidth);
}

void Engine::resetLayers() {
	backgroundLayer.reset();
	tileLayer.reset();
}

Bitmap Engine::createBitmap(std::uint32_t width, std::uint32_t height) {
	return m_renderer.createBitmap(width, height);
}

void Engine::destroyBitmap(Bitmap b) {
	m_renderer.destroyBitmap(b);
}

void Engine::setTargetBitmap(Bitmap b) {
	m_renderer.setTargetBitmap(b);
}

void Engine::clearBitmap(Bitmap b, Color c) {
	m_renderer.setTargetBitmap(b);
	m_renderer.clear(c);
}

void Engine::clearCurrentBitmap(Color c) {
	m_renderer.clear(c);
}

void Engine::drawBitmap(Bitmap b, int x, int y) {
	m_renderer.drawBitmap(b, x, y);
}

void Engine::clearDisplay() {
	m_renderer.clear(Color::RGB(0, 0, 0));
}

void Engine::updateDisplay() {
	m_renderer.updateDisplay();
}

void Engine::makeDisplayCurrentBitmap() {
	m_renderer.makeDisplayCurrentBitmap(display);
}

Event* Engine::processInput() {
	m_renderer.waitForEvent(eventQueue, &ev);
	return &ev;
}

void Engine::loadFont(const std::string& id, const std::string& fontFullPath, int size) {
	if(m_fontMap.find(id) != m_fontMap.end()) {
		m_logger->trace("loadFont: font '{}' already loaded, skipping", id);
		return;
	}

	auto myFont = m_renderer.loadFont(fontFullPath, size);
	assert(myFont);
	m_fontMap[id] = myFont;
	m_logger->debug("Font '{}' loaded from '{}'", id, fontFullPath);
}

Color Engine::color(byte red, byte green, byte blue) {
	return Color::RGB(red, green, blue);
}

TileLayer* Engine::getTileLayer() const {
	return tileLayer.get();
}

BackgroundLayer* Engine::getBackgroundLayer() const {
	return backgroundLayer.get();
}

Display Engine::getDisplay() const {
	return display;
}

ViewData& Engine::getViewData() {
	return viewData;
}

std::uint32_t Engine::getMapWidth() const {
	return mapWidth;
}

std::uint32_t Engine::getMapHeight() const {
	return mapHeight;
}

EventQueue Engine::getEventQueue() const {
	return eventQueue;
}

Timer Engine::getFPSTimer() const {
	return FPSTimer;
}

Timer Engine::getLogicTimer() const {
	return logicTimer;
}

double Engine::getRenderFps() const {
	return FPS;
}

Font Engine::getFont(const std::string& id) const {
	const auto font = m_fontMap.find(id);
	if(font == m_fontMap.end()) {
		return nullptr;
	}
	return font->second;
}

void Engine::drawText(const char* textToDraw, int x, int y, Font font, Color color) {
	if(textToDraw == nullptr) {
		return;
	}
	if(font == nullptr) {
		font = m_defaultFont;
	}
	m_renderer.drawText(font, color, x, y, Flip::NONE, textToDraw);
}

RandomGenerator& Engine::getRandomGenerator() {
	return m_randomGenerator;
}

} // namespace ValyrianEngine
