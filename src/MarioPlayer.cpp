#include "MarioPlayer.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/Engine.hpp>
#include <Engine/Logger.hpp>
#include <Engine/SoundManager.hpp>
#include <Engine/SystemClock.hpp>

const std::array<std::string, 3> MarioPlayer::s_sizeStrings{{"SMALL", "SUPER", "FIERY"}};
const std::array<std::string, 2> MarioPlayer::s_generalStateStrings{{"STARTMAN", "NONE"}};

MarioPlayer::MarioPlayer(int _x, int _y, Size s, Direction dir, State _state)
    : Player(_x, _y, s_sizeStrings[static_cast<int>(s)], dir, _state),
      m_size(s) {
	m_logger->trace("MarioPlayer constructor");

	gravityHandler.SetOnStartFalling([&]() {
		if(!m_isJumping) {
			startFalling();
		}
	});

	gravityHandler.SetOnStopFalling([&]() {
		m_jumpDownAnimator->stop();
	});
}

MarioPlayer::~MarioPlayer() {
	m_logger->trace("Starting MarioPlayer destruction");
}

void MarioPlayer::initialize() {
	initializeAnimations();
}

void MarioPlayer::setXForOneTileCrossed(int s) {
	m_xSpeedForOneTileCrossed = s;
}

void MarioPlayer::setXForTwoTileCrossed(int s) {
	m_xSpeedForTwoTileCrossed = s;
}

void MarioPlayer::setOriginalXSpeed(int s) {
	m_originalXSpeed = s;
}

void MarioPlayer::enableRun() {
	m_isRunning = true;
}

void MarioPlayer::move(int& dx, int& dy) {
	if(!m_moveAnimator->isRunnig() && !m_isJumping && !gravityHandler.isFalling) {
		m_stopNext = false;
		m_state = State::MOVING;
		updateFilm();
		m_moveAnimator->start(m_moveAnimation.get(), getSystemTime());
	}

	Sprite::move(dx, dy);
}

void MarioPlayer::stopMoving() {
	if(m_moveAnimator->isRunnig()) {
		m_moveAnimator->stop();
		m_state = State::IDLE;
		updateFilm(false);
	}

	if(!gravityHandler.isFalling && !m_isJumping) {
		m_state = State::IDLE;
		updateFilm();
	}
}

bool MarioPlayer::canStartMoveAnim() const {
	return !m_isJumping && !gravityHandler.isFalling;
}

void MarioPlayer::hit() {
	if(m_isInvurable) {
		m_logger->warn("Can not hit mario he is invurable");
		return;
	}

	if(m_size == Size::SMALL) {
		decreaseLife();
	} else if(m_size == Size::SUPER) {
		makeInvulnerable(15, 0.15);
		setSize(Size::SMALL);
	} else if(m_size == Size::FIRE) {
		makeInvulnerable(15, 0.15);
		setSize(Size::SUPER);
	}
}

void MarioPlayer::setSize(Size s) {
	auto oldScreenH = frameBox.screenHeight;

	m_size = s;
	m_filmPrefix = s_sizeStrings[static_cast<int>(s)];
	film = AnimationFilmHolder::getInstance().getFilm(filmName(m_state));
	frameBox = film->getFrameBox(0);

	// Keep the bottom of the sprite pinned to the same world position.
	// oldScreenH - newScreenH is positive when shrinking (y moves down)
	// and negative when growing (y moves up).
	y += oldScreenH - frameBox.screenHeight;
}

MarioPlayer::Size MarioPlayer::getSize() const {
	return m_size;
}

int MarioPlayer::getLifes() const {
	return m_lifes;
}

int MarioPlayer::getCoins() const {
	return m_coins;
}

bool MarioPlayer::isJumping() const {
	return m_isJumping;
}

bool MarioPlayer::isFalling() const {
	return gravityHandler.isFalling;
}

bool MarioPlayer::isInvulnerable() const {
	return m_isInvurable;
}

void MarioPlayer::increaseCoins() {
	m_coins += 1;
	if(m_coins == m_coinsForLife) {
		m_lifes++;
		m_coins = 0;
	}
}

void MarioPlayer::stopRun() {
	m_tilesCrossedBeforeStopping = 0;
	m_startingX = 0;
	m_xSpeed = m_originalXSpeed;
	m_moveAnimation->setDelay(m_moveFrameDelay);
	m_isRunning = false;
}

void MarioPlayer::startJumping() {
	if(!m_jumpUpAnimator->isRunnig() && m_canJumpAgain && !m_jumpDownAnimator->isRunnig()) {
		{
			auto dy = m_jumpUpAnimation->getDy();
			auto dx = 0;
			s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
			if(dy == 0) {
				m_logger->debug("Mario can not jump dy == 0");
				return;
			}
		}

		m_jumpUpAnimator->start(m_jumpUpAnimation.get(), getSystemTime());
		m_stopNext = false;
		m_canJumpAgain = false;

		auto volume = SoundManager::getInstance().getVolume();
		auto volumeToPlay = volume * 0.8;

		if(m_size == Size::SMALL) {
			volumeToPlay = volume == 0.0 ? volume : 0.05;
			SoundManager::getInstance().playSound("jumpSMALL", PlayMode::ONCE, volumeToPlay);
		} else {
			SoundManager::getInstance().playSound("jumpSUPER", PlayMode::ONCE, volumeToPlay);
		}
	}
}

void MarioPlayer::startFalling() {
	if(!m_jumpUpAnimator->isRunnig() && !m_jumpDownAnimator->isRunnig()) {
		{
			auto dy = m_jumpDownAnimation->getDy();
			auto dx = 0;
			s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
			if(dy == 0) {
				m_state = State::IDLE;
				updateFilm();
				m_logger->debug("Mario can not fall dy == 0");
				return;
			}
		}

		m_logger->debug(__FUNCTION__);
		film = AnimationFilmHolder::getInstance().getFilm(s_sizeStrings[static_cast<int>(m_size)] + "_JUMPING");
		assert(film);
		frameBox = film->getFrameBox(0);
		m_jumpDownAnimator->start(m_jumpDownAnimation.get(), getSystemTime());
	}
}

void MarioPlayer::makeInvulnerable(float secs, float hideDelay) {
	if(!m_flashAnimator->isRunnig()) {
		m_flashAnimation = std::make_unique<FlashAnimation>("FlashAnim", secs, hideDelay);
		m_flashAnimator->start(m_flashAnimation.get(), getSystemTime());
	}
}

void MarioPlayer::bounce() {
	if(!m_jumpUpAnimator->isRunnig()) {
		m_jumpDownAnimator->stop();
		m_jumpUpAnimator->start(m_movingBounceAnimation.get(), getSystemTime());
		m_playJumpMusic = false;
	}
}

void MarioPlayer::startFlagDown() {
	if(!m_moveDownTheFlagAnimator->isRunnig()) {
		m_inputDisabled = true;
		m_moveDownTheFlagAnimator->start(m_moveDownTheFlagAnimation.get(), getSystemTime());
	}
}

void MarioPlayer::stopJump() {
	if(m_jumpUpAnimator->isRunnig() && !m_jumpDownAnimator->isRunnig()) {
		m_state = State::IDLE;
		m_jumpUpAnimator->stop();
		m_stopNext = false;
	}
}

void MarioPlayer::resetJump() {
	m_canJumpAgain = true;
}
