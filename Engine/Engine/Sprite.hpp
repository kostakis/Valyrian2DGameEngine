#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "AnimationFilm.hpp"
#include "DestructionManager.hpp"
#include "GravityHandler.hpp"
#include "Grid.hpp"
#include "Rectangle.hpp"
#include "Renderer.hpp"
#include "SystemClock.hpp"
#include "ViewData.hpp"

#include <array>
#include <memory>
#include <string>

namespace ValyrianEngine {

class Sprite : public LatelyDestroyable {
public:
	static std::shared_ptr<Grid> s_grid;
	static const std::array<std::string, 4> stateStrings;
	static const std::array<std::string, 2> directionStrings;

	enum class Direction { RIGHT = 0, LEFT };

	enum class State { IDLE = 0, MOVING, RUNNING, ROPE };

public:
	Sprite() = default;
	Sprite(int _x, int _y, const std::string& typeId, const std::string& state, AnimationFilm* animFilm = nullptr);
	virtual ~Sprite();

	/*Setters*/
	virtual void setIsVisible(bool visible) {
		isVisible = visible;
	}
	void setdirectMotion(bool motion) {
		directMotion = motion;
	}
	void setZOrder(unsigned z) {
		zOrder = z;
	}
	void setTypeId(std::string type) {
		typeId = type;
	}
	void setStateId(std::string state) {
		stateId = state;
	}
	void setX(int _x) {
		x = _x;
	}
	void setY(int _y) {
		y = _y;
	}
	void setDirection(Direction dir) {
		m_direction = dir;
	}

	/*Getters*/
	bool getIsVisible() const;
	bool getdirectMotion() const;
	unsigned getZOrder() const;
	const std::string& getTypeId() const;
	const std::string& getStateId() const;
	const Rectangle& getRectangle() const;
	int getX() const;
	int getY() const;
	Rectangle getScreenRectangle() const;
	Direction getDirection() const;
	GravityHandler& getGravityHandler();

	/*Misc*/
	virtual bool collisionCheck(const Sprite* s) const;
	virtual void changeDirection();
	virtual void startAnimation(uint64_t time = getSystemTime());
	virtual void renderRec(Renderer& renderer, int xOff, int yOff, const ViewData& vw);
	virtual void move(int& dx, int& dy);
	virtual void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw);
	virtual bool hasAnimationStarted() const {
		return m_hasAnimationStarted;
	}

protected:
	GravityHandler gravityHandler;

	int x{};
	int y{};
	Direction m_direction{Direction::LEFT};
	AnimationFilm* film{nullptr};
	Rectangle frameBox{}; // Current frame
	Bitmap m_bitmap{nullptr};

	bool isVisible{true};
	bool directMotion{false};
	bool isMoving{false}; // Has animator started ?

	unsigned zOrder{0};
	std::string typeId{};
	std::string stateId{};
	bool m_hasAnimationStarted{false};
};

} // namespace ValyrianEngine
#endif
