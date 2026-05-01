#ifndef GAME_HPP
#define GAME_HPP

#include "Goomba.hpp"
#include "KoopaTroopa.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Grid.hpp>
#include <Engine/Player.hpp>
#include <Engine/Sprite.hpp>

#include <tmxlite/Map.hpp>

#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace ValyrianEngine {

struct textToRender {
	const char* txt;
	int x{0};
	int y{0};
	Font font{nullptr};
	Color color{};
};

class Game {
public:
	static constexpr std::array<const char*, 1> s_allTilesAsSpritesStr{"Brick"};

	/*State*/
	enum class State { Playing, Pause, Quit };

	enum class PlayerKind { MARIO, GENERIC };

	Game(std::shared_ptr<Engine> en);
	~Game();

	void main();

	void initialize(const std::string&, const std::set<int>& solidTileIds, const std::vector<std::string>&,
	                const std::string& tmxPath, int brickTileId);

	void createPlayer(PlayerKind kind, const std::string& filmPrefix);

	void initFromGeneralConfig(const std::string& engineConfigPath, const std::string& levelConfigPath);
	void initEnemies(const std::string& tmxPath);
	TileCollisionShapes loadTileCollisionShapes(const std::string& tmxPath);

	enum class CameraMode {
		MARIO, // Horizontal only, scroll at center
		FREE   // Follow player in all directions
	};

	std::shared_ptr<Engine> getEngine() const;
	Player* getPlayer() const;
	State getState() const;

	void addDrawTextToList(const textToRender& t);

private:
	CameraMode m_cameraMode{CameraMode::MARIO};
	std::shared_ptr<spdlog::logger> m_logger;

	/*View Data*/
	ViewData& m_viewData;

	/*Tile Stuff*/
	byte m_tileLayerTileWidth;
	byte m_tileLayerTileHeight;
	int m_maxTileForHeightTileLayer;
	int m_maxTileWidthTileLayer;

	// The .tmx file holding all the data
	tmx::Map enemiesTmx;

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

	bool keysState[static_cast<int>(Keys::COUNT)] = {false};
	State state;

	/*Engine*/
	std::shared_ptr<Engine> engine;
	Event* ev{nullptr};

	/*Grid*/
	std::shared_ptr<Grid> grid;
	bool renderGrid{false};
	bool renderGridSubCells{false};
	bool enableJump{true};
	bool enableDownMovement{false};
	int m_brickTileId{-1};

	/*Offsets*/
	int yRenderOffset{0}; /*Y offset when display the bitmap at the display window*/
	int xRenderOffset{0}; /*X offset when display the bitmap at the display window*/

	/*Scroll speeds*/
	bool reDraw{false};
	int horizontalScrollSpeed{1};

	/*Game variables*/
	bool finished{false};

	/*Private methods*/
	void getInput();
	void processInput();
	void progressAnimations();

	/*Tests*/
	Rectangle testRec{128, 96, 16, 16};

	/*Player*/
	Player* m_player{nullptr};
	PlayerKind m_playerKind{PlayerKind::MARIO};
	int x_starting{0};
	int y_starting{0};

	/*Frames*/
	uint64_t prevTime{0};
	uint64_t currTime{0};
	double fps{0};
	double frameCounter{0};
	bool renderFps{false};

	/*General Methods*/
	void mainLoopIteration();
	void mainLoop();

	void checkMoving() const; // Rename ME
	void initSounds() const;

	Bitmap m_bufferBitmap;

	std::chrono::system_clock::time_point end;

	std::list<textToRender> m_drawTextList;

	/*Threads*/
	std::thread soundsInitThread;

	/*Misc*/

	void render();
	void checkDead();
	void physics();
	void ai();
	void input();
};

} // namespace ValyrianEngine
#endif
