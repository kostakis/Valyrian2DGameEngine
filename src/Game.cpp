#include "Game.hpp"
#include "Brick.hpp"
#include "Coin.hpp"
#include "EndRope.hpp"
#include "ICollsionWithPlayer.hpp"
#include "MarioPlayer.hpp"
#include "NpcBrick.hpp"
#include "TerminationFlag.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/AnimatorManager.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/ConfigReader.hpp>
#include <Engine/Logger.hpp>
#include <Engine/SoundManager.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>

#include <tmxlite/Tileset.hpp>

#include <algorithm>
#include <cstdlib>

namespace ValyrianEngine {

void Game::getInput() {
	ev = engine->processInput();
}

Game::Game(std::shared_ptr<Engine> en)
    : engine(en),
      m_viewData(en->getViewData()),
      m_tileLayerTileWidth(en->getTileLayer()->getScaledTileWidth()),
      m_tileLayerTileHeight(en->getTileLayer()->getScaledTileHeight()),
      m_maxTileForHeightTileLayer(static_cast<int>(en->getTileLayer()->getRows())),
      m_maxTileWidthTileLayer(static_cast<int>(en->getTileLayer()->getColumns())) {
	m_logger = getOrCreateLogger("Game");
	m_logger->trace("Game constructor");

	grid = std::make_shared<Grid>();
	m_bufferBitmap = en->createBitmap(Engine::s_windowWidth, Engine::s_windowHeight);
	assert(m_bufferBitmap);
}

Game::~Game() {
	m_logger->trace("~Game() Started");

	AnimatorManager::getInstance().stopAll();
	m_logger->debug("All animators stopped");

	if(soundsInitThread.joinable()) {
		soundsInitThread.join();
		m_logger->debug("Sound initialization thread joined");
	}

	if(m_player) {
		if(m_player->getAlive()) {
			m_player->PrepareForManualDelete();
			delete m_player;
		}
		m_player = nullptr;
		m_logger->debug("Player object cleaned up");
	}

	m_logger->debug("Sprites alive before cleanup: {}", SpriteManager::getInstance().getSpriteCount());

	DestructionManager::getInstance().commit(); // Clear backlog
	SpriteManager::getInstance().CleanUp();     // Delete remaining sprites

	m_logger->debug("Sprites alive after cleanup: {}", SpriteManager::getInstance().getSpriteCount());
	m_logger->debug("SpriteManager cleanup");

	CollisionChecker::getInstance().Clear();
	m_logger->debug("CollisionChecker cleanup");

	Sprite::s_grid.reset();
	m_logger->debug("Global grid pointer reset");

	SoundManager::getInstance().stopAllSounds();
	m_logger->debug("SoundManager cleanup");

	DestructionManager::getInstance().Clear(); // Reset the list
	m_logger->debug("DestructionManager cleanup");

	AnimationFilmHolder::getInstance().cleanUp(); // Animations must be removed last
	m_logger->debug("AnimationFilmHolder cleanup");

	m_viewData.x = 0;
	engine->destroyBitmap(m_bufferBitmap);

	m_logger->trace("~Game() Finished");
}

void Game::initialize(const std::string& startScreenPath, const std::set<int>& solidTileIds,
                      const std::vector<std::string>& allConfigPaths, const std::string& tmxPath, int brickTileId) {
	m_brickTileId = brickTileId;
	/*Sounds*/
	soundsInitThread = std::thread(&Game::initSounds, this);

	/*Grid*/
	grid->Initialize(engine->getTileLayer());
	grid->setSolidTiles(solidTileIds);
	grid->setTileCollisionShapes(loadTileCollisionShapes(tmxPath));
	grid->computeGrid();
	Sprite::s_grid = grid;

	/*Films*/
	for(const auto& path : allConfigPaths) {
		AnimationFilmHolder::getInstance().loadAll(path);
	}

	/*Keys*/
	for(int i = 0; i < static_cast<int>(Keys::COUNT); i++) {
		keysState[i] = false;
	}

	/*Fonts*/
	engine->loadFont("fpsFont", "media/fonts/CrimsonText-Bold.ttf", 22);
	engine->loadFont("marioFont", "media/fonts/SuperMario256.ttf", 16);

	/*View Data*/
	engine->getViewData().updateX(0);

	/*Game State*/
	state = Game::State::Playing;

	/*Times*/
	prevTime = getSystemTime();
}

void Game::createPlayer(PlayerKind kind, const std::string& filmPrefix) {
	m_playerKind = kind;
	if(kind == PlayerKind::MARIO) {
		m_player = new MarioPlayer(4, 50);
	} else {
		m_player = new Player(4, 50, filmPrefix);
	}
}

void Game::initFromGeneralConfig(const std::string& engineConfigPath, const std::string& levelConfigPath) {
	auto getVal = [&](const char* key) {
		const char* val = nullptr;
		if(!levelConfigPath.empty()) {
			val = ConfigReader::getInstance().getValue(levelConfigPath, "", key);
		}
		if(!val) {
			val = ConfigReader::getInstance().getValue(engineConfigPath, "", key);
		}
		if(!val) {
			m_logger->error("Configuration key '{}' not found in level config ({}) or engine config ({})", key,
			               levelConfigPath, engineConfigPath);
			assert(false && "Configuration key not found");
		}
		return val;
	};

	auto x_speed = getVal("X_SPEED");
	auto y_speed = getVal("Y_SPEED");
	auto x_scroll_speed = getVal("X_SCROLL_SPEED");
	auto render_Fps = getVal("SHOW_FPS");
	auto render_GridSubCells = getVal("SHOW_GRID_SUBCELLS");
	auto enable_Jump = getVal("ENABLE_JUMP");
	auto enable_DownMovement = getVal("ENABLE_DOWN_MOVEMENT");
	auto gravity = getVal("ENABLE_GRAVITY");
	bool gravity1 = strcmp(gravity, "1") == 0 ? true : false;
	auto soundVolume = getVal("SOUND_VOLUME");
	x_starting = std::atoi(getVal("X_STARTING"));
	y_starting = std::atoi(getVal("Y_STARTING"));

	m_player->setXSpeed(std::atoi(x_speed));
	m_player->setYSpeed(std::atoi(y_speed));
	m_player->getGravityHandler().gravityAddicted = gravity1;

	// Mario-specific knobs (size, run speeds) only apply when the active player is MarioPlayer.
	if(auto* mp = dynamic_cast<MarioPlayer*>(m_player)) {
		auto mario_size = getVal("MARIO_SIZE");
		auto x_speed_1 = getVal("X_SPEED_1");
		auto x_speed_2 = getVal("X_SPEED_2");
		mp->setOriginalXSpeed(std::atoi(x_speed));
		mp->setXForOneTileCrossed(std::atoi(x_speed_1));
		mp->setXForTwoTileCrossed(std::atoi(x_speed_2));
		mp->setSize(static_cast<MarioPlayer::Size>(std::atoi(mario_size)));
	}
	auto cameraMode = getVal("CAMERA_MODE");
	m_cameraMode = (strcmp(cameraMode, "1") == 0) ? CameraMode::FREE : CameraMode::MARIO;

	auto clampToMap = getVal("CLAMP_TO_MAP_BOUNDS");
	grid->setClampToMapBounds(clampToMap && strcmp(clampToMap, "1") == 0);

	horizontalScrollSpeed = std::atoi(x_scroll_speed);
	renderFps = strcmp(render_Fps, "1") == 0 ? true : false;
	renderGridSubCells = render_GridSubCells && strcmp(render_GridSubCells, "1") == 0;
	enableJump = !enable_Jump || strcmp(enable_Jump, "0") != 0;
	enableDownMovement = enable_DownMovement && strcmp(enable_DownMovement, "1") == 0;
	SoundManager::getInstance().setVolume(std::stof(soundVolume));

	m_player->setX(x_starting);
	m_player->setY(y_starting);
	m_player->initialize();
}

TileCollisionShapes Game::loadTileCollisionShapes(const std::string& tmxPath) {
	TileCollisionShapes shapes;

	tmx::Map tilesetsTmx;
	if(!tilesetsTmx.load(tmxPath)) {
		m_logger->warn("Tile collision: cannot load {}, no per-tile shapes will be applied", tmxPath);
		return shapes;
	}

	for(const auto& tileset : tilesetsTmx.getTilesets()) {
		auto firstGid = tileset.getFirstGID();
		for(const auto& tile : tileset.getTiles()) {
			const auto& objs = tile.objectGroup.getObjects();
			if(objs.empty())
				continue;

			std::vector<TileShape> rects;
			rects.reserve(objs.size());
			for(const auto& obj : objs) {
				const auto& aabb = obj.getAABB();
				rects.push_back({aabb.left, aabb.top, aabb.width, aabb.height});
			}
			shapes[tile.ID] = std::move(rects);
		}
	}

	m_logger->info("Tile collision: loaded shapes for {} tiles from {}", shapes.size(), tmxPath);
	return shapes;
}

void Game::initEnemies(const std::string& tmxPath) {
	if(!enemiesTmx.load(tmxPath)) {
		m_logger->error("Enemies file {} does not exists. Enemies will not be created", tmxPath);
		return;
	}

	std::vector<std::pair<int, int>> npcBricks{};

	const auto& layers = enemiesTmx.getLayers();
	for(const auto& layer : layers) {
		if(layer->getType() == tmx::Layer::Type::Object) {
			if(layer->getName() == "Enemies") {
				KoopaTroopa::s_koopaTroopaFactory(layer);

				const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
				const auto& objects = objectLayer.getObjects();
				for(const auto& object : objects) {
					if(object.getType() == "Goomba") {
						createGoomba(object, *this);
					} // TODO create goomba factory
				}
			} else if(layer->getName() == "SpritesAsActions") {
				Coin::s_coinFactory(layer); // Create coins

				const auto& objectLayer = layer->getLayerAs<tmx::ObjectGroup>();
				const auto& objects = objectLayer.getObjects();
				for(const auto& object : objects) {
					if(object.getType() == "BrickSprite") {
						int x = object.getPosition().x;
						int y = object.getPosition().y;
						new Brick(x * 2, (y - 16) * 2, *this, engine->getRenderer());
						engine->getTileLayer()->setInvisibleTiles((y - 16) / 16, x / 16);
					} else if(object.getType() == "NpcBrick") {
						int x = object.getPosition().x;
						int y = object.getPosition().y;
						assert(y % 16 == 0);
						assert(x % 16 == 0);
						int occur = 0;
						auto& properties = object.getProperties();
						bool visibility{true};
						for(const auto& prop : properties) {
							if(prop.getName() == "NpcType") {
							} else if(prop.getName() == "Occurences") {
								occur = prop.getIntValue();
							} else if(prop.getName() == "Visibility") {
								visibility = prop.getBoolValue();
							}
						}
						new NpcBrick(x * 2, (y - 16) * 2, NpcBrick::NpcType::COIN, occur, *this, visibility);
						npcBricks.push_back(std::make_pair(x + 16, y - 16)); // Some nasty stuff going on...
						engine->getTileLayer()->setInvisibleTiles((y - 16) / 16, x / 16);
					} else if(object.getType() == "EndRope") {
						m_logger->trace("Found EndRope Sprite");
						int x = object.getPosition().x;
						int y = object.getPosition().y;
						new EndRope(x * 2, (y - 16) * 2);

					} else if(object.getType() == "TerminationFlag") {
						m_logger->trace("Found TerminationFlag Sprite");
						int x = object.getPosition().x;
						int y = object.getPosition().y;
						// assert(y % 16 == 0);
						// assert(x % 16 == 0);
						int factor = 1;
						int tilesDown = 1;

						auto properites = object.getProperties();
						for(const auto& prop : properites) {
							if(prop.getName() == "scaleFactor") {
								factor = prop.getIntValue();
							} else if(prop.getName() == "tilesDown") {
								tilesDown = prop.getIntValue();
							}
						}

						new TerminationFlag(x * factor, (y - 16) * factor, tilesDown);
					}
				}
			}
		}
	}

	if(m_brickTileId >= 0) {
		const auto& tile = engine->getTileLayer()->getTilesWithId(m_brickTileId);
		auto width = engine->getTileLayer()->getScaledTileWidth();
		auto height = engine->getTileLayer()->getScaledTileHeight();

		for(auto i = 0; i < tile.size(); i++) {
			auto x = tile[i].first;
			auto y = tile[i].second;
			auto myPair = std::make_pair(x, y);
			if(std::find(npcBricks.begin(), npcBricks.end(), myPair) == npcBricks.end()) {
				new Brick(x, y, *this, engine->getRenderer());
				engine->getTileLayer()->setInvisibleTiles(y / width, x / height);
			}
		}
	}

	const auto& mapWithAllSprites = SpriteManager::getInstance().getAllList();
	for(const auto& myMapPair : mapWithAllSprites) {
		for(auto* sprite : myMapPair.second) {
			Enemy* tmpEnemy = dynamic_cast<Enemy*>(sprite);
			if(tmpEnemy != nullptr) {
				tmpEnemy->initCollision();
			}

			ICollsionWithPlayer* playerCollisionSprite = dynamic_cast<ICollsionWithPlayer*>(sprite);
			if(playerCollisionSprite != nullptr) {
				playerCollisionSprite->registerCollision();
			}
		}
	}
}

std::shared_ptr<Engine> Game::getEngine() const {
	return engine;
}

Player* Game::getPlayer() const {
	return m_player;
}

Game::State Game::getState() const {
	return state;
}

void Game::addDrawTextToList(const textToRender& t) {
	m_drawTextList.push_back(t);
}

void Game::mainLoopIteration() {
	getInput();
	processInput();

	input(); // Input not related to the game. Ex pause window

	// physics();
	ai();
	progressAnimations();

	if(reDraw) {
		render();
	}

	CollisionChecker::getInstance().Check();
	DestructionManager::getInstance().commit();

	end = std::chrono::system_clock::now();

	// Should the game end?
	checkDead();
}

void Game::mainLoop() {
	static auto start = std::chrono::system_clock::now();
	while(!finished) {
		{ // Fps calculation
			// currTime = getSystemTime();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
			if(elapsed.count() >= 1) { // Update the fps coutner after 1 sec
				// std::cout << fps << std::endl;
				fps = frameCounter;
				frameCounter = 0;
				/*auto delta = currTime - prevTime;
				fps = (1.00f / delta) * (engine->getRenderFps() * 10);
				*/
				start = end;
				m_drawTextList.clear();
			}
		}
		// prevTime = currTime;
		mainLoopIteration();
	}
	m_logger->info("mainLoop finished");
}

void Game::checkMoving() const {}

void Game::initSounds() const {
	SoundManager::getInstance().addSound("MainSound", "media/sounds/Overworld.ogg");
	SoundManager::getInstance().addSound("LoseLife", "media/sounds/Lose a Life.ogg");
	SoundManager::getInstance().addSound("BonusRoom", "media/sounds/BonusRoom.ogg");
	SoundManager::getInstance().addSound("jumpSMALL", "media/sounds/mario_jump_small.ogg");
	SoundManager::getInstance().addSound("jumpSUPER", "media/sounds/mario_jump-super.ogg");
	SoundManager::getInstance().addSound("Pause", "media/sounds/Pause.ogg");
	SoundManager::getInstance().addSound("Coin", "media/sounds/Coin.ogg");
	SoundManager::getInstance().addSound("Pipe", "media/sounds/Pipe.ogg");
	SoundManager::getInstance().addSound("Powerup", "media/sounds/Powerup.ogg");
	SoundManager::getInstance().addSound("Powerup_appears", "media/sounds/Powerup_appears.ogg");
	SoundManager::getInstance().addSound("Worldclear", "media/sounds/World_clear.ogg");
	SoundManager::getInstance().addSound("Gameover", "media/sounds/Gameover.ogg");
	SoundManager::getInstance().addSound("1-up", "media/sounds/1-up.ogg");
	SoundManager::getInstance().addSound("Bump", "media/sounds/Bump.ogg");
	// SoundManager::getInstance().addSound("Kick", "media/sounds/Kick.ogg"); // Can not be loaded at linux
	SoundManager::getInstance().addSound("Stage_clear", "media/sounds/Stage_clear.ogg");
	SoundManager::getInstance().addSound("Flagpole", "media/sounds/Flagpole.ogg");
	SoundManager::getInstance().addSound("Stomp", "media/sounds/Stomp.ogg");
	SoundManager::getInstance().addSound("Blockhit", "media/sounds/blockhit.ogg");
	SoundManager::getInstance().addSound("Starman", "media/sounds/Starman.ogg");
	SoundManager::getInstance().addSound("BreakBrick", "media/sounds/smb_breakblock.ogg");
}

void Game::render() {
	auto& renderer = engine->getRenderer();

	// Clear the current screen bitmap
	engine->clearCurrentBitmap();

	// Layers
	engine->getBackgroundLayer()->render(renderer, xRenderOffset, yRenderOffset, m_viewData);
	engine->getTileLayer()->render(renderer, xRenderOffset, yRenderOffset, m_viewData);

	// Grid
	if(renderGrid) {
		grid->Render(renderer, yRenderOffset, m_viewData, renderGridSubCells);
		m_player->renderRec(renderer, xRenderOffset, yRenderOffset, m_viewData);
	}

	// All sprites (including player)
	SpriteManager::getInstance().renderAll(renderer, xRenderOffset, yRenderOffset, m_viewData, renderGrid);

	if(renderFps) {
		engine->drawText(std::to_string(static_cast<int>(fps)).c_str(), Engine::s_windowWidth - 100, 100,
		                 engine->getFont("fpsFont"), engine->color(255, 0, 0));
	}

	if(auto* mp = dynamic_cast<MarioPlayer*>(m_player)) {
		engine->drawText(std::to_string(static_cast<int>(mp->getLifes())).c_str(), Engine::s_windowWidth / 2, 10,
		                 engine->getFont("marioFont"), engine->color(255, 255, 255));
	}

	for(const auto& text : m_drawTextList) {
		auto font = text.font == nullptr ? engine->getFont("marioFont") : text.font;
		engine->drawText(text.txt, text.x - m_viewData.x, text.y - m_viewData.y, font, text.color);
	}

	// Flip/Update The display
	engine->updateDisplay();
	frameCounter++;
	reDraw = false;

	// m_logger->trace("render finished"); // Be carefull with this
}

void Game::checkDead() {
	// Sprite::move auto-Destroy()s any sprite that falls past the screen, including the
	// Player. Once that happens DestructionManager::commit() frees the Player and our
	// m_player pointer dangles. SpriteManager is the source of truth, so check it first.
	if(m_player && SpriteManager::getInstance().getTypeList(Player::s_type).empty()) {
		m_player = nullptr;
		finished = true;
		return;
	}

	if(!m_player) {
		finished = true;
		return;
	}

	if(!m_player->getAlive()) {
		m_logger->warn("Player is dead game should finish");
		finished = true;
	}

	if(auto* mp = dynamic_cast<MarioPlayer*>(m_player)) {
		if(mp->getLifes() <= 0) {
			m_logger->warn("Player is out of lifes game should finish");
			finished = true;
		}
	}
}

void Game::physics() {
	// ?????
}

void Game::ai() {
	for(const auto& enemyStr : Enemy::allEnemysStr) {
		auto& enemiesList = SpriteManager::getInstance().getTypeList(enemyStr); // Maybe better way???
		for(auto& sprite : enemiesList) {
			if(!sprite->hasAnimationStarted() && m_viewData.x2 >= sprite->getX()) {
				sprite->startAnimation(getSystemTime());
				m_logger->debug("Starting sprite: " + sprite->getTypeId());
			}

			// If it goes out of the sceen(on the left) destroy it
			if(sprite->getX() + engine->getTileLayer()->getScaledTileWidth() <
			   m_viewData.x) { // Give some room before disappearing
				sprite->Destroy();
			}

			// If it falls off the bottom of the map, destroy it
			if(sprite->getY() > Engine::s_windowHeight) {
				sprite->Destroy();
			}
		}

		auto& coinList = SpriteManager::getInstance().getTypeList(Coin::s_type); // Maybe better way???
		auto time = getSystemTime();
		for(auto& sprite : coinList) {
			if(!sprite->hasAnimationStarted() && m_viewData.x2 >= sprite->getX()) {
				sprite->startAnimation(time);
				m_logger->debug("Starting sprite: " + sprite->getTypeId());
			}
		}
	}
}

void Game::input() {
	// TODO
}

void Game::main() {
	soundsInitThread.join();
	m_logger->info("Sounds initiated");
	SoundManager::getInstance().playSound("MainSound", PlayMode::LOOP);
	m_logger->info("Starting main game loop");
	mainLoop();
}

void Game::progressAnimations() {
	// m_logger->trace("progressAnimations");
	AnimatorManager::getInstance().progress(getSystemTime());
}

} // namespace ValyrianEngine
