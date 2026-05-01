#ifndef KOOPATROOPA_HPP
#define KOOPATROOPA_HPP

#include "Enemy.hpp"
#include "FordwardDeclarations.hpp"
#include "ICollsionWithPlayer.hpp"
#include "KoopaTroopa.hpp"
#include "MarioPlayer.hpp"

#include <Engine/CollisionChecker.hpp>
#include <Engine/SpriteManager.hpp>
#include <Engine/SystemClock.hpp>
#include <Engine/ViewData.hpp>

#include <tmxlite/ObjectGroup.hpp>

using namespace ValyrianEngine;

class KoopaTroopa : public Enemy {
public:
	enum class State { MOVING, STUNNED, KICKED };

public:
	static void s_koopaTroopaFactory(const tmx::Layer::Ptr& spritesLayer);

	KoopaTroopa(int x, int y);
	~KoopaTroopa();

	void initialize() override;
	void initializeAnimations() override;
	void startAnimation(uint64_t time = getSystemTime()) override;
	void changeDirection() override;
	void initCollision() override;
	void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) override;

	// ICollsionWithPlayer
	void registerCollision() override;

	// Getters
	State getState() const;

private:
	int m_dx{6};
	int m_dy{0};
	int m_frameSwitchDelay{100};
	State m_state{State::MOVING};

	Bitmap m_koopaBitmap{nullptr};

	void changeState(State newState);
};

#endif
