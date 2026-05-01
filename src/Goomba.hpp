#ifndef GOOMBA_HPP
#define GOOMBA_HPP

#include "Enemy.hpp"
#include "ICollsionWithPlayer.hpp"
#include "MarioPlayer.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/BitmapManager.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/FrameRangeAnimation.hpp>
#include <Engine/FrameRangeAnimator.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>
#include <Engine/ViewData.hpp>

#include <tmxlite/ObjectGroup.hpp>

#include <functional>
#include <iostream>
#include <memory>

namespace ValyrianEngine {

class Game;

void createGoomba(const tmx::Object& object, Game& game);

class Goomba : public Enemy {
public:
	Goomba(int x, int y, Game& game);
	~Goomba();

	void initialize() override;
	void startAnimation(uint64_t time = getSystemTime()) override;
	void initializeAnimations() override;
	void changeDirection() override;
	void initCollision() override;
	void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) override;

	// ICollsionWithPlayer
	void registerCollision() override;

private:
	Game& m_game;

	int m_dx{4};
	int m_dy{0};
	int m_frameSwitchDelay{115};
	bool m_brickCollision{false};
	int m_randomFallDegree{90};
	Bitmap bitmap{nullptr};
};

} // namespace ValyrianEngine
#endif
