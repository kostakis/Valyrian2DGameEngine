#include "GameManager.hpp"
#include "Game.hpp"

#include <Engine/Logger.hpp>

#include <chrono>

GameManager::GameManager() {
	m_logger = getOrCreateLogger("GameManager");
	m_logger->trace("GameManager constructor");
	m_engine = std::make_shared<Engine>();
}

GameManager::~GameManager() {
	m_logger->trace("GameManager destructor");
	m_engine.reset();
	m_game.reset();
}

void GameManager::initialize() {
	/*Keys*/
	for(bool& i : keysState) {
		i = false;
	}

	m_engine->initialize();
	m_engine->createWindow();
	m_engine->loadFont("myMarioFont", "media/fonts/SuperMario256.ttf", 26);
	m_startScreenBitmap = BitmapManager::getInstance().loadBitmap("startBitmap", m_startScreenPath);
}

void GameManager::main() {
	m_logger->info("GameManger::main()");
	// For now, initialize a default level config.
	// This can later be populated by a menu selection or a JSON world file.
	MapConfig dungeonMapConfig = {
	    "media/maps/TinyDungeonLevel1FunMap/sampleMap_Dungeon.csv", // tileLayer
	    "",                                                         // backgroundLayer (none)
	    "media/maps/TinyDungeonLevel1FunMap/sampleMap.tmx",         // tmxConfiguration
	    "config/general.cfg",                                       // engineConfig
	    "media/maps/TinyDungeonLevel1FunMap/dungeon_level1.cfg",     // levelConfig
	    "media/spritesAndTilesResources/kenneyTilesSprites/Dungeon/Tilemap/tilemap_packed.png", // tilesetPath and sprites for dungeon
	    11,                                                                                     // tilesetRows
	    12,                                                                                     // tilesetCols
	    {1,  2,  4,  5,  6,  7,          8,  9,  10, 11,                                        
	     23, 35, 36, 47,                                                                        
	     13, 14, 15, 16, 17, 18,         19, 20, 21, 22,                                        
	     25, 26, 27, 28, 29, 30,         31, 32, 33, 34,                                        
	     37, 38, 39, 40, 41, /*42,*/ 43, 44, 45, 46,                                            
	     57, 59}, // Solid tiles
	    {"media/spritesAndTilesResources/kenneyTilesSprites/Dungeon/kenney_player.cfg"},
	    -1,                        // brickTileId - not used
	    Game::PlayerKind::GENERIC, // playerKind (top-down dungeon player)
	    "PLAYER"                   // playerFilmPrefix
	};

	MapConfig marioMapConfig = {
	    "media/maps/SuperMarioLevel1FunMap/TileLayer.csv",                        // tileLayer
	    "media/maps/SuperMarioLevel1FunMap/BackgroundLayer.csv",                  // backgroundLayer
	    "media/maps/SuperMarioLevel1FunMap/FirstLevel.tmx",                       // tmxConfiguration
	    "config/general.cfg",                                                     // engineConfig
	    "media/maps/SuperMarioLevel1FunMap/mario_level1.cfg",                     // levelConfig
	    "media/spritesAndTilesResources/marioTilesSprites/super_mario_tiles.png", // tilesetPath
	    12,                                                                       // tilesetRows
	    19,                                                                       // tilesetCols
	    {0,  1,  2,  3,  22, 23, 41,  42,  48,  60,  61,  79,  80, 98, 99,
	     76, 77, 78, 95, 96, 97, 114, 115, 116, 133, 134, 135, 84, 85, 86}, // solidTiles
	    {"media/spritesAndTilesResources/marioTilesSprites/marioAnimations.cfg",
	     "media/spritesAndTilesResources/marioTilesSprites/enemiesAnimations.cfg",
	     "media/spritesAndTilesResources/marioTilesSprites/tilesAsSprites.cfg",
	     "media/spritesAndTilesResources/marioTilesSprites/sprites.cfg"}, // animationConfigs
	    48,                                                               // brickTileId
	    Game::PlayerKind::MARIO,
	};

	// Switch active level here:
	// m_currentMapConfig = dungeonMapConfig;
	m_currentMapConfig = dungeonMapConfig;

	while(!m_exit) {
		m_currTime = std::chrono::steady_clock::now();
		if(!m_disableInput) {
			input();
		}
		if(m_state == GameState::START_SCREEN) {
			m_engine->clearCurrentBitmap(Color::RGB(0, 155, 217));
			auto bitmapPositionx =
			    ::Engine::Engine::s_windowWidth / 2 - m_engine->getRenderer().getBitmapWidth(m_startScreenBitmap) / 2;
			auto bitmapPositiony =
			    ::Engine::Engine::s_windowHeight / 2 - m_engine->getRenderer().getBitmapHeight(m_startScreenBitmap) / 2;

			m_engine->drawBitmap(m_startScreenBitmap, bitmapPositionx, bitmapPositiony);

			m_engine->updateDisplay();
		} else if(m_state == GameState::LOADING) {
			m_engine->clearCurrentBitmap(Color::RGB(0, 0, 0));
			m_engine->drawText(GameManager::s_loadingScreenTip, ::Engine::Engine::s_windowWidth / 2 - 250,
			                   ::Engine::Engine::s_windowHeight - 70, m_engine->getFont("myMarioFont"));
			m_engine->updateDisplay();
			if(!m_startedInit) {
				m_startedInit = true;
				initGame(m_currentMapConfig); // Load the chosen level
				m_startedLoadingTime = std::chrono::steady_clock::now();
			}
			if(m_startedInit) {
				auto elapsed =
				    std::chrono::duration_cast<std::chrono::seconds>(m_currTime - m_startedLoadingTime).count();
				if(elapsed >= m_loadtime) {
					m_state = GameState::PLAYING;
				}
			}
		} else if(m_state == GameState::PLAYING) {
			m_logger->info("Starting game");
			m_game->main();
			if(m_game->getState() == Game::State::Quit) {
				m_exit = true;
				m_logger->info("Quit game");
			} else {
				m_disableInput = false;
				m_startedInit = false;
				m_game.reset();
				m_engine->resetLayers();
				// TODO must check current player lifes......
				m_state = GameState::START_SCREEN;
			}
		}
	}
}

void GameManager::input() {
	ev = m_engine->processInput();
	switch(ev->type) {
		case EventType::KEY_DOWN:
			switch(ev->keyboard.keycode) {
				case KeyCode::ESCAPE:
					m_exit = true;
					break;
				case KeyCode::ENTER:
					m_state = GameState::LOADING;
					m_disableInput = true;
					break;
				case KeyCode::PAD_ENTER:
					m_state = GameState::LOADING;
					m_disableInput = true;
					break;
				default:
					break;
			}
			break;
		case EventType::DISPLAY_CLOSE:
			m_exit = true;
			break;
		default:
			break;
	}
}

void GameManager::initGame(const MapConfig& config) {
	m_engine->initializeTileLayer(16, 16, config.tilesetPath, config.tileLayer, config.tilesetRows, config.tilesetCols);
	m_engine->getTileLayer()->setScalling(32, 32);
	m_engine->initializeBackgroundLayer(16, 16, config.tilesetPath, config.backgroundLayer, config.tilesetRows,
	                                    config.tilesetCols);

	m_game = std::make_unique<Game>(m_engine);
	m_game->initialize(m_startScreenPath, config.solidTiles, config.animationConfigs, config.tmxConfiguration,
	                   config.brickTileId);

	m_game->createPlayer(config.playerKind, config.playerFilmPrefix); // Player must be created first
	m_game->initEnemies(config.tmxConfiguration);
	m_game->initFromGeneralConfig(config.engineConfig, config.levelConfig);
}
