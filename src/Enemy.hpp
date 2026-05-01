#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "FordwardDeclarations.hpp"
#include "ICollsionWithPlayer.hpp"

#include <Engine/Sprite.hpp>
#include <Engine/SystemClock.hpp>

#include <array>
#include <memory>

namespace ValyrianEngine {

class Enemy : public Sprite, public ICollsionWithPlayer {
public:
	static constexpr const char* s_goombaType = "Goomba";
	static constexpr const char* s_koopaType = "Koopa";

	static constexpr std::array<const char*, 2> allEnemysStr{"Goomba", "Koopa"};

	Enemy(int x, int y, const std::string& id, AnimationFilm* animFilm);
	~Enemy();

	void startFalling();
	void stopFalling();
	void bounceEnemyFromUp();

	virtual void initialize() = 0;
	virtual void initializeAnimations() {}
	virtual void startAnimation(uint64_t time = getSystemTime()) {}
	virtual void initCollision() = 0;
	virtual void fireOrBrickCollision();

	void initSameTypeCollision(const std::string& typeId);

protected:
	// Falling
	std::unique_ptr<MovingAnimator> m_fallingAnimator{nullptr};
	std::unique_ptr<MovingAnimation> m_fallingAnimation{nullptr};

	// Walk
	std::unique_ptr<FrameRangeAnimator> m_frameRangeAnimator{nullptr};
	std::unique_ptr<FrameRangeAnimation> m_frameRangeAnimation{nullptr};

	// State-change / death
	std::unique_ptr<MovingAnimator> m_movingAnimator{nullptr};
	std::unique_ptr<MovingAnimation> m_movingAnimation{nullptr};

	// Brick collision
	std::unique_ptr<MovingAnimator> m_brickCollisionAnimator{nullptr};
	std::unique_ptr<MovingAnimation> m_brickCollisionAnimation{nullptr};

	bool m_canHurtMario{true};
	bool m_isBouncing{false};
};

} // namespace ValyrianEngine
#endif
