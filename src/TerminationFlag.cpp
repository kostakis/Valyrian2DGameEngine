#include "TerminationFlag.hpp"
#include "MarioPlayer.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/BitmapManager.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/MovingAnimator.hpp>
#include <Engine/SpriteManager.hpp>

TerminationFlag::TerminationFlag(int x, int y, int tilesDown)
    : Sprite(x, y, s_terminFlagType, "Created", AnimationFilmHolder::getInstance().getFilm("TerminationFlag")) {
	m_logger = getOrCreateLogger("TerminationFlag");
	m_logger->trace("TerminationFlag constructor");
	m_tilesDown = tilesDown;
	isVisible = true;
	directMotion = true;
	m_bitmap = BitmapManager::getInstance().getBitmap("sprites");

	initializeAnimations();
}

TerminationFlag::~TerminationFlag() {
	m_logger->trace("TerminationFlag destructor");
	if(m_moveAnimator->isRunnig()) {
		m_moveAnimator->stop();
	}
}

void TerminationFlag::registerCollision() {
	// Player collision
	// TODO end game - play end map animation all that stuff ...
	// Play music ....

	auto* player = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front();
	CollisionChecker::getInstance().Register(this, player, [&](Sprite* coin, Sprite* player) {
		CollisionChecker::getInstance().Cancel(this);
		startAnimation();
	});
}

void TerminationFlag::startAnimation(uint64_t time) {
	if(!m_moveAnimator->isRunnig()) {
		m_moveAnimator->start(m_moveAnim.get(), getSystemTime());
		m_hasAnimationStarted = true;
	}
}

void TerminationFlag::initializeAnimations() {
	m_moveAnimator = std::make_unique<MovingAnimator>();

	int pixelsToMove = 2;
	m_moveAnim = std::make_unique<MovingAnimation>("TerminationFlagAnim", (m_tilesDown * 32) / pixelsToMove, 0,
	                                               pixelsToMove, 20);

	m_moveAnimator->setOnStart([&](Animator*) {
		directMotion = true;
		m_logger->debug("Started framerange animator");
	});

	m_moveAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto& animation = (MovingAnimation&)(anim);

		int dx = 0;
		int dy = animation.getDy();

		move(dx, dy);
		m_logger->debug("Moved {}", dy);
	});

	m_moveAnimator->setOnFinish([&](Animator*) {
		m_logger->debug("Stopped framerange animator");

		Destroy();
	});
}
