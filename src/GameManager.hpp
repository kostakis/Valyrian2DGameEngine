#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "FordwardDeclarations.hpp"
#include "Game.hpp"

#include <Engine/Engine.hpp>

#include <chrono>
#include <memory>
#include <string>

using namespace ValyrianEngine;

struct MapConfig {
	std::string tileLayer;
	std::string backgroundLayer;
	std::string tmxConfiguration;
	std::string engineConfig{"config/general.cfg"};
	std::string levelConfig;
	std::string tilesetPath;
	std::uint32_t tilesetRows;
	std::uint32_t tilesetCols;
	std::set<int> solidTiles;
	std::vector<std::string> animationConfigs;
	int brickTileId{-1}; // tile id whose static cells should be replaced with Brick sprites; -1 disables
	Game::PlayerKind playerKind{};
	std::string playerFilmPrefix{"PLAYER"}; // ignored for MARIO (size string drives the prefix)
};

class GameManager {
public:
	enum GameState {
		START_SCREEN,
		LOADING,
		PLAYING,
	};

	GameManager();
	~GameManager();

	void initialize();

	void main();

private:
	enum class Keys {
		KEY_UP,
		KEY_DOWN,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_HOME,
		KEY_END,
		MOUSE_LEFT_BUTTON,
		KEY_G,
		KEY_A,
		KEY_W,
		KEY_S,
		KEY_D,
		KEY_Z,
		ENTER,
		SPACEBAR,
		F1_KEY,
		LSHIFT,
		COUNT
	};

	static constexpr char s_loadingScreenTip[] = "Remember you can run with LSHIFT";
	static constexpr char s_startScreenTip[] = "Press enter to start";

	std::shared_ptr<spdlog::logger> m_logger;

	bool keysState[static_cast<int>(Keys::COUNT)] = {false};

	std::shared_ptr<Engine> m_engine;
	std::unique_ptr<Game> m_game;

	Bitmap m_startScreenBitmap{nullptr};
	std::string m_startScreenPath{"media/engine_start.png"};

	int m_lvl{1};

	bool m_exit{false};
	bool m_disableInput{false};
	bool m_startedInit{false};
	GameState m_state{START_SCREEN};

	bool m_redraw{true};

	Event* ev{nullptr};

	std::chrono::steady_clock::time_point m_startedLoadingTime;
	std::chrono::steady_clock::time_point m_currTime;
	int m_loadtime{1}; // seconds

	MapConfig m_currentMapConfig{};

	void input();
	void initGame(const MapConfig& config);
};

#endif
