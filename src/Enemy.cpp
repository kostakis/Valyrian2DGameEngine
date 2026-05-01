#include "Enemy.hpp"
#include "Game.hpp"
#include "MarioPlayer.hpp"

#include <Engine/CollisionChecker.hpp>
#include <Engine/Logger.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>

namespace ValyrianEngine {

Enemy::Enemy(int x, int y, const std::string& id, AnimationFilm* animFilm)
    : Sprite(x, y, id, "moving", animFilm) {
	m_logger = getOrCreateLogger("Enemy");

	gravityHandler.SetOnStartFalling([this]() {
		startFalling();
	});
	gravityHandler.SetOnStopFalling([this]() {
		stopFalling();
	});
}

Enemy::~Enemy() {}

void Enemy::startFalling() {
	if(m_isBouncing)
		return;

	m_fallingAnimator = std::make_unique<MovingAnimator>();
	m_fallingAnimation = std::make_unique<MovingAnimation>("Falling", 0, 0, 4, 16);

	m_fallingAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto moveAnim = (MovingAnimation&)anim;
		int dx = moveAnim.getDx();
		int dy = moveAnim.getDy();
		Sprite::move(dx, dy);
	});

	m_fallingAnimator->start(m_fallingAnimation.get(), getSystemTime());
}

void Enemy::stopFalling() {
	if(m_fallingAnimator && m_fallingAnimator->isRunnig())
		m_fallingAnimator->stop();
}

void Enemy::bounceEnemyFromUp() {
	auto* myPlayer = static_cast<MarioPlayer*>(SpriteManager::getInstance().getTypeList(MarioPlayer::s_type).front());
	myPlayer->bounce();

	m_isBouncing = true;
	m_fallingAnimator = std::make_unique<MovingAnimator>();
	m_fallingAnimation = std::make_unique<MovingAnimation>("Bounce", 0, 0, -3, 30);

	m_fallingAnimator->setOnAction([&](Animator* a, const Animation& anim) {
		auto moveAnim = (MovingAnimation&)anim;
		int dx = moveAnim.getDx();
		int dy = moveAnim.getDy();

		if(m_fallingAnimator->getCurrentRep() >= 15) {
			dy = -dy;
		}

		Sprite::move(dx, dy);
	});

	m_fallingAnimator->setOnFinish([&](Animator* a) {
		m_isBouncing = false;
	});

	m_fallingAnimator->start(m_fallingAnimation.get(), getSystemTime());
}

void Enemy::fireOrBrickCollision() {
	if(m_brickCollisionAnimator && !m_brickCollisionAnimator->isRunnig())
		m_brickCollisionAnimator->start(m_brickCollisionAnimation.get(), getSystemTime());
}

void Enemy::initSameTypeCollision(const std::string& typeId) {
	const auto& list = SpriteManager::getInstance().getTypeList(typeId);
	for(const auto& other : list) {
		if(other != this) {
			CollisionChecker::getInstance().Register(other, this, [](Sprite* s1, Sprite* s2) {
				if(s1->getX() < s2->getX()) {
					if(s1->getDirection() == Direction::RIGHT)
						s1->changeDirection();
					if(s2->getDirection() == Direction::LEFT)
						s2->changeDirection();
				} else {
					if(s1->getDirection() == Direction::LEFT)
						s1->changeDirection();
					if(s2->getDirection() == Direction::RIGHT)
						s2->changeDirection();
				}
			});
		}
	}
}

} // namespace ValyrianEngine
