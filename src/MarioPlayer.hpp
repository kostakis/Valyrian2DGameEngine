#ifndef MARIOPLAYER_HPP
#define MARIOPLAYER_HPP

#include "FordwardDeclarations.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/BitmapManager.hpp>
#include <Engine/FlashAnimation.hpp>
#include <Engine/FlashAnimator.hpp>
#include <Engine/MovingAnimator.hpp>
#include <Engine/Player.hpp>
#include <Engine/ViewData.hpp>

#include <array>
#include <memory>
#include <string>

using namespace ValyrianEngine;

class MarioPlayer : public ValyrianEngine::Player {
public:
	enum class Size { SMALL = 0, SUPER, FIRE };
	static const std::array<std::string, 3> s_sizeStrings;

	enum class GeneralState {
		STARTMAN,
		NONE,
	};
	static const std::array<std::string, 2> s_generalStateStrings;

	MarioPlayer(int x, int y, Size s = Size::SMALL, Direction dir = Direction::RIGHT, State state = State::IDLE);
	~MarioPlayer() override;

	void initialize() override;

	/* Mario-specific setters */
	void setXForOneTileCrossed(int s);
	void setXForTwoTileCrossed(int s);
	void setOriginalXSpeed(int s);
	void enableRun();

	void setSize(Size s);
	Size getSize() const;
	int getCoins() const;
	int getLifes() const;
	bool isJumping() const;
	bool isFalling() const;
	bool isInvulnerable() const;

	/* Animations */
	void startJumping();
	void startFalling();
	void makeInvulnerable(float secs = 1, float hideDelay = 0.2f);
	void bounce();
	void startFlagDown();

	/* Misc */
	void increaseCoins();
	void stopRun();
	void stopJump();
	void resetJump();
	void decreaseLife();
	void hit();

	void move(int& dx, int& dy) override;
	void stopMoving() override;

protected:
	bool canStartMoveAnim() const override;
	void onMoveAnimStart() override;
	void onMoveAnimFrame(unsigned currFrame) override;
	void onMoveAnimFinish() override;

private:
	static constexpr const char* s_animationsConfig = "config/marioAnimationsConfiguration.cfg";

	int m_coins{0};
	static constexpr int m_coinsForLife{100};
	int m_lifes{1};

	int m_originalXSpeed{0};
	int m_xSpeedForOneTileCrossed{0};
	int m_xSpeedForTwoTileCrossed{0};

	int m_tilesCrossedBeforeStopping{0};
	int m_startingX{0};

	Size m_size{Size::SMALL};
	GeneralState m_generalState{GeneralState::NONE};

	int m_frameRangeDelayOneTile{70};
	int m_frameRangeDelayTwoTiles{60};

	bool m_isRunning{false};
	bool m_isJumping{false};
	bool m_canJumpAgain{true};
	bool m_canJumpMax{false};
	bool m_isInvurable{false};
	bool m_playJumpMusic{true};
	bool m_stopNext{false};

	std::shared_ptr<MovingAnimator> m_jumpUpAnimator;
	std::shared_ptr<MovingAnimator> m_jumpDownAnimator;
	std::unique_ptr<FlashAnimator> m_flashAnimator;
	std::unique_ptr<MovingAnimator> m_movingBounceAnimator;
	std::unique_ptr<MovingAnimator> m_moveDownTheFlagAnimator;
	std::unique_ptr<MovingAnimator> m_walkToCastleAnimator;

	std::shared_ptr<MovingAnimation> m_jumpUpAnimation;
	std::shared_ptr<MovingAnimation> m_jumpDownAnimation;
	std::unique_ptr<FlashAnimation> m_flashAnimation;
	std::unique_ptr<MovingAnimation> m_movingBounceAnimation;
	std::unique_ptr<MovingAnimation> m_moveDownTheFlagAnimation;
	std::unique_ptr<MovingAnimation> m_walkToCastleAnimation;

	void initializeAnimations() override;
};

#endif
