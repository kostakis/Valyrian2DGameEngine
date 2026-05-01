#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "FrameRangeAnimation.hpp"
#include "FrameRangeAnimator.hpp"
#include "Sprite.hpp"
#include "ViewData.hpp"

#include <memory>
#include <string>

namespace spdlog {
class logger;
}

namespace ValyrianEngine {

// Game-agnostic player.
// Provides:
//   * grid-collision-aware movement (move/stopMoving)
//   * IDLE / MOVING film flipping using a configurable film-name prefix
//   * a frame-range walk animation
// Subclass for game-specific behavior (e.g. Mario's jump, size, lives).
class Player : public Sprite {
public:
	static constexpr const char* s_type = "player";

	Player(int x, int y, const std::string& filmPrefix = "PLAYER", Direction dir = Direction::RIGHT,
	       State state = State::IDLE);
	~Player() override;

	virtual void initialize();

	void setXSpeed(int v);
	void setYSpeed(int v);
	int getXSpeed() const;
	int getYSpeed() const;

	bool isInputDisabled() const;
	void setInputDisabled(bool b);

	void move(int& dx, int& dy) override;
	virtual void stopMoving();
	void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) override;

protected:
	std::string m_filmPrefix;
	int m_xSpeed{0};
	int m_ySpeed{0};
	State m_state{State::IDLE};
	bool m_inputDisabled{false};

	int m_moveFrameDelay{70};

	std::unique_ptr<FrameRangeAnimator> m_moveAnimator;
	std::unique_ptr<FrameRangeAnimation> m_moveAnimation;

	std::shared_ptr<spdlog::logger> m_logger;

	// Subclass hooks for the walk animator.
	virtual bool canStartMoveAnim() const;
	virtual void onMoveAnimStart();
	virtual void onMoveAnimFrame(unsigned currentFrame);
	virtual void onMoveAnimFinish();

	void updateFilm(bool changeFrame = true);
	std::string filmName(State s) const;

	virtual void initializeAnimations();
};

} // namespace ValyrianEngine

#endif
