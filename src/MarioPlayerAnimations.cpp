#include "MarioPlayer.hpp"
#include "TerminationFlag.hpp"

#include <Engine/CollisionChecker.hpp>
#include <Engine/ConfigReader.hpp>
#include <Engine/Engine.hpp>
#include <Engine/MyDebug.hpp>
#include <Engine/SoundManager.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>

#include <spdlog/spdlog.h>

void MarioPlayer::decreaseLife() {
	m_logger->info("Loosing life");
	m_logger->info("m_isInvurable: {}", m_isInvurable);
	if(!m_isInvurable) {
		m_lifes--;
	}
}

void MarioPlayer::initializeAnimations() {
	// Base sets up m_moveAnimator + m_moveAnimation and wires the hooks below.
	Player::initializeAnimations();

	m_movingBounceAnimator = std::make_unique<MovingAnimator>();
	m_jumpUpAnimator = std::make_shared<MovingAnimator>();
	m_jumpDownAnimator = std::make_shared<MovingAnimator>();

	// clang-format off
	m_jumpUpAnimation = std::make_shared<MovingAnimation>(
	    "JumpUpAnimation",
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpUpRepetitions")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpUpDx")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpUpDy")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpUpFrameDelay")));

	m_jumpDownAnimation = std::make_shared<MovingAnimation>(
	    "JumpDownAnimation",
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpDownRepetitions")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpDownDx")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpDownDy")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpDownFrameDelay")));

	m_movingBounceAnimation = std::make_unique<MovingAnimation>(
	    "JumpBounceAnimation",
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpBounceRepetitions")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpBounceDx")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpBounceDy")),
	    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpBounceFrameDelay")));
	// clang-format on

	m_jumpUpAnimator->setOnStart([&](Animator* a) {
		m_logger->info("Player jump up animator started");
		film = AnimationFilmHolder::getInstance().getFilm(s_sizeStrings[static_cast<int>(m_size)] + "_JUMPING");
		assert(film);
		frameBox = film->getFrameBox(0);

		m_isJumping = true;
		if(m_canJumpMax) {
			m_jumpUpAnimation->setReps(
			    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpMaxUpRepetitions")));
		}

		m_logger->info("Will play for reps: " + m_jumpUpAnimation->getReps());
	});

	m_jumpUpAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto myAnimator = reinterpret_cast<FrameRangeAnimator*>(a);
		auto myAnimation = (MovingAnimation&)(anim);

		auto dy = myAnimation.getDy();
		auto dx = myAnimation.getDx();

		if(m_stopNext) {
			m_logger->debug("Will stop jump due to previous dy==0");
			m_stopNext = false;
			myAnimator->stop();
		}

		s_grid->FilterGridMotion(getScreenRectangle(), &dx, &dy);
		if(dy == 0) {
			m_logger->debug("dy==0 while jumpning will stop next iteration");
			m_stopNext = true;
		} else {
			Sprite::move(dx, dy);
		}
	});

	m_jumpUpAnimator->setOnFinish([&](Animator* a) {
		m_canJumpMax = false;
		m_stopNext = false;
		m_isJumping = false;
		m_jumpUpAnimation->setReps(
		    std::atoi(ConfigReader::getInstance().getConfigValue(s_animationsConfig, "JumpUpRepetitions")));
		m_logger->info("Player jump up animator stopped");
		startFalling();
	});

	m_jumpDownAnimator->setOnStart([&](Animator* a) {
		m_logger->info("Player jump down animator started");
		gravityHandler.isFalling = true;
		stopMoving();
	});

	m_jumpDownAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto myAnimation = (MovingAnimation&)(anim);

		auto dy = myAnimation.getDy();
		auto dx = myAnimation.getDx();
		Sprite::move(dx, dy);
	});

	m_jumpDownAnimator->setOnFinish([&](Animator* a) {
		m_jumpUpAnimator->stop();
		m_logger->info("Player jump down animator stopped");
		gravityHandler.isFalling = false;
		m_state = State::IDLE;
		updateFilm();
	});

	m_flashAnimator = std::make_unique<FlashAnimator>();

	m_flashAnimator->setOnStart([&](Animator* a) {
		m_logger->info("Flash animator started");
		m_isInvurable = true;
	});

	m_flashAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto toShow = m_flashAnimator->getState();
		isVisible = toShow ? true : false;
	});

	m_flashAnimator->setOnFinish([&](Animator* a) {
		isVisible = true;
		m_isInvurable = false;
		m_flashAnimator->reset();
	});

	m_moveDownTheFlagAnimator = std::make_unique<MovingAnimator>();
	m_moveDownTheFlagAnimation = std::make_unique<MovingAnimation>("MoveDown Animation", 0, 0, 2, 20);

	m_moveDownTheFlagAnimator->setOnStart([&](Animator* a) {
		const auto& flags = SpriteManager::getInstance().getTypeList(TerminationFlag::s_terminFlagType);
		for(Sprite* flag : flags) {
			flag->startAnimation();
		}

		m_moveAnimator->stop();
		m_jumpUpAnimator->stop();
		m_jumpDownAnimator->stop();
		gravityHandler.gravityAddicted = false;
		gravityHandler.isFalling = false;
		m_isJumping = false;
		m_state = State::ROPE;
		updateFilm(true);
		m_logger->debug("Started move down flag animator");
		m_direction = Direction::RIGHT;
		x += 16;
	});

	m_moveDownTheFlagAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto& animation = (MovingAnimation&)(anim);

		auto dx = animation.getDx();
		auto dy = animation.getDy();

		Sprite::move(dx, dy);

		if(dy == 0) {
			m_logger->debug("Reached the end of flag rope");
			a->stop();
		}
	});

	m_moveDownTheFlagAnimator->setOnFinish([&](Animator* a) {
		m_logger->debug("Stopped move down flag animator");
		gravityHandler.gravityAddicted = true;
		gravityHandler.isFalling = false;
		m_walkToCastleAnimator->start(m_walkToCastleAnimation.get(), getSystemTime());
	});

	m_walkToCastleAnimator = std::make_unique<MovingAnimator>();
	m_walkToCastleAnimation = std::make_unique<MovingAnimation>("WalkToCastle Animation", 0, 2, 0, 20);

	m_walkToCastleAnimator->setOnStart([&](Animator* a) {
		setDirection(Direction::RIGHT);
		m_state = State::MOVING;
		updateFilm(true);
		m_moveAnimator->start(m_moveAnimation.get(), getSystemTime());
		m_logger->debug("Started walk-to-castle animator");
	});

	m_walkToCastleAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto& animation = (MovingAnimation&)(anim);
		int dx = animation.getDx();
		int dy = animation.getDy();
		Sprite::move(dx, dy);
	});

	m_walkToCastleAnimator->setOnFinish([&](Animator* a) {
		m_logger->debug("Stopped walk-to-castle animator");
	});
}

void MarioPlayer::onMoveAnimStart() {
	m_startingX = x;
	m_logger->debug("Frame range player started");
}

void MarioPlayer::onMoveAnimFrame(unsigned currFrame) {
	if(m_isRunning) {
		int diff{0};
		if(m_direction == Direction::RIGHT) {
			diff = x - m_startingX;
			if(diff >= 50)
				m_tilesCrossedBeforeStopping++;
		} else {
			diff = m_startingX - x;
			if(diff >= 50)
				m_tilesCrossedBeforeStopping++;
		}

		if(m_tilesCrossedBeforeStopping == 1) {
			m_xSpeed = m_xSpeedForOneTileCrossed;
			m_moveAnimation->setDelay(m_frameRangeDelayOneTile);
		} else if(m_tilesCrossedBeforeStopping >= 2) {
			m_logger->debug("Max jump enabled");
			m_xSpeed = m_xSpeedForTwoTileCrossed;
			m_canJumpMax = true;
			m_moveAnimation->setDelay(m_frameRangeDelayTwoTiles);
		}
	}
}

void MarioPlayer::onMoveAnimFinish() {
	m_tilesCrossedBeforeStopping = 0;
	m_startingX = 0;
	m_xSpeed = m_originalXSpeed;
	m_moveAnimation->setDelay(m_moveFrameDelay);
	m_logger->debug("Frame range player stopped");
}
