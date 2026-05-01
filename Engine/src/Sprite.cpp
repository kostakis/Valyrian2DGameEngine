#include "Sprite.hpp"
#include "BitmapManager.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include "MyDebug.hpp"
#include "SpriteManager.hpp"

namespace ValyrianEngine {

std::shared_ptr<Grid> Sprite::s_grid = nullptr;

const std::array<std::string, 4> Sprite::stateStrings = {"IDLE", "MOVING", "RUNNING", "ROPE"};
const std::array<std::string, 2> Sprite::directionStrings = {"RIGHT", "LEFT"};

Sprite::Sprite(int _x, int _y, const std::string& _typeId, const std::string& _stateId, AnimationFilm* animFilm)
    : x(_x),
      y(_y),
      typeId(_typeId),
      stateId(_stateId) {
	m_logger = getOrCreateLogger("Sprite");
	if(animFilm != nullptr) {
		film = animFilm;
		frameBox = film->getFrameBox(0); // Just the first frame at construction
	}
	SpriteManager::getInstance().Add(this);
	gravityHandler.SetOnSolidGround([&](const Rectangle rec) {
		return s_grid->isOnSolidGround(rec);
	});
	gravityHandler.SetOnStartFalling([]() {
	});
	gravityHandler.SetOnStopFalling([]() {
	});
}

Sprite::~Sprite() {
	m_logger->trace("Sprite destructor: {}", typeId);
	SpriteManager::getInstance().Remove(this);
}

void Sprite::renderRec(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	auto color = Color::RGB(0, 0, 255);
	renderer.drawRectangle(x + xOff - vw.x, y + yOff - vw.y, x + xOff + frameBox.screenWidth - vw.x,
	                       y + yOff + frameBox.screenHeight - vw.y, color, 2.00f);
}

void Sprite::move(int& dx, int& dy) {
	if(directMotion) {
		x += dx;
		y += dy;
	} else {
		// Grid
		s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
		x += dx;
		y += dy;

		// Gravity
		gravityHandler.Check(getScreenRectangle());

		// Not sure if this is the best place to check this ...
		if(y >= Engine::s_windowHeight) {
			Destroy();
		}
	}
}

void Sprite::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	renderer.drawScaledBitmap(m_bitmap, frameBox.x, frameBox.y, frameBox.width, frameBox.height, x + xOff - vw.x,
	                          y + yOff - vw.y, frameBox.screenWidth, frameBox.screenHeight, Flip::NONE);
}

bool Sprite::getIsVisible() const {
	return isVisible;
}

bool Sprite::getdirectMotion() const {
	return directMotion;
}

unsigned Sprite::getZOrder() const {
	return zOrder;
}

const std::string& Sprite::getTypeId() const {
	return typeId;
}

const std::string& Sprite::getStateId() const {
	return stateId;
}

const Rectangle& Sprite::getRectangle() const {
	return frameBox;
}

int Sprite::getX() const {
	return x;
}

int Sprite::getY() const {
	return y;
}

Rectangle Sprite::getScreenRectangle() const {
	return Rectangle{x, y, frameBox.screenWidth, frameBox.screenHeight};
}

Sprite::Direction Sprite::getDirection() const {
	return m_direction;
}

GravityHandler& Sprite::getGravityHandler() {
	return gravityHandler;
}

bool Sprite::collisionCheck(const Sprite* s) const {
	//// http://jeffreythompson.org/collision-detection/rect-rect.php
	if(x + frameBox.screenWidth > s->getX() &&          // r1 right edge past r2 left
	   x < s->getX() + s->getRectangle().screenWidth && // r1 left edge past r2 right
	   y + frameBox.screenHeight >= s->getY() &&        // r1 top edge past r2 bottom
	   y <= s->getY() + s->getRectangle().screenHeight) // r1 bottom edge past r2 top
	{
		return true;
	}
	return false;
}

void Sprite::changeDirection() {
	m_direction = m_direction == Direction::LEFT ? Direction::RIGHT : Direction::LEFT;
}

void Sprite::startAnimation(uint64_t time) {}

} // namespace ValyrianEngine
