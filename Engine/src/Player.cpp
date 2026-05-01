#include "Player.hpp"

#include "AnimationFilmHolder.hpp"
#include "Logger.hpp"
#include "SystemClock.hpp"

#include <cassert>

namespace ValyrianEngine {

Player::Player(int _x, int _y, const std::string& filmPrefix, Direction dir, State _state)
    : Sprite(_x, _y, s_type, stateStrings[static_cast<int>(_state)],
             AnimationFilmHolder::getInstance().getFilm(filmPrefix + "_" + stateStrings[static_cast<int>(_state)])),
      m_filmPrefix(filmPrefix),
      m_state(_state) {
	m_logger = getOrCreateLogger("Player");
	m_logger->trace("Player base constructor (prefix={})", filmPrefix);
	m_direction = dir;
}

Player::~Player() = default;

void Player::initialize() {
	initializeAnimations();
}

void Player::initializeAnimations() {
	m_moveAnimator = std::make_unique<FrameRangeAnimator>();
	auto* movingFilm = AnimationFilmHolder::getInstance().getFilm(filmName(State::MOVING));
	assert(movingFilm);
	int lastFrame = movingFilm->getTotalFrames() - 1;
	m_moveAnimation = std::make_unique<FrameRangeAnimation>("PlayerMoving", 0, lastFrame, 0, 0, 0, m_moveFrameDelay);

	m_moveAnimator->setOnStart([this](Animator*) {
		onMoveAnimStart();
	});

	m_moveAnimator->setOnAction([this](Animator* a, const Animation&) {
		auto* fra = reinterpret_cast<FrameRangeAnimator*>(a);
		if(canStartMoveAnim()) {
			m_state = State::MOVING;
			updateFilm(false);
			frameBox = film->getFrameBox(fra->getCurrFrame());
		}
		onMoveAnimFrame(fra->getCurrFrame());
	});

	m_moveAnimator->setOnFinish([this](Animator*) {
		onMoveAnimFinish();
	});
}

bool Player::canStartMoveAnim() const {
	return true;
}

// Default empty implementations
void Player::onMoveAnimStart() {}
void Player::onMoveAnimFrame(unsigned) {}
void Player::onMoveAnimFinish() {}

void Player::setXSpeed(int v) {
	m_xSpeed = v;
}
void Player::setYSpeed(int v) {
	m_ySpeed = v;
}
int Player::getXSpeed() const {
	return m_xSpeed;
}
int Player::getYSpeed() const {
	return m_ySpeed;
}
bool Player::isInputDisabled() const {
	return m_inputDisabled;
}
void Player::setInputDisabled(bool b) {
	m_inputDisabled = b;
}

void Player::move(int& dx, int& dy) {
	if(m_moveAnimator && !m_moveAnimator->isRunnig() && canStartMoveAnim()) {
		m_state = State::MOVING;
		updateFilm();
		m_moveAnimator->start(m_moveAnimation.get(), getSystemTime());
	}
	Sprite::move(dx, dy);
}

void Player::stopMoving() {
	if(m_moveAnimator && m_moveAnimator->isRunnig()) {
		m_moveAnimator->stop();
		m_state = State::IDLE;
		updateFilm(false);
	}
	if(!gravityHandler.isFalling && canStartMoveAnim()) {
		m_state = State::IDLE;
		updateFilm();
	}
}

void Player::render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) {
	if(!isVisible || !film)
		return;
	const Flip flip = (m_direction == Direction::LEFT) ? Flip::HORIZONTAL : Flip::NONE;

	// Support both scaled and region rendering agnostically
	renderer.drawScaledBitmap(film->getBitmap(), frameBox.x, frameBox.y, frameBox.width, frameBox.height,
	                          x + xOff - vw.x, y + yOff - vw.y, frameBox.screenWidth, frameBox.screenHeight, flip);
}

std::string Player::filmName(State s) const {
	return m_filmPrefix + "_" + stateStrings[static_cast<int>(s)];
}

void Player::updateFilm(bool changeFrame) {
	auto* f = AnimationFilmHolder::getInstance().getFilm(filmName(m_state));
	assert(f);
	film = f;
	if(changeFrame) {
		frameBox = film->getFrameBox(0);
	}
}

} // namespace ValyrianEngine
