#ifndef TERMINATIONFLAG_HPP
#define TERMINATIONFLAG_HPP

#include "FordwardDeclarations.hpp"
#include "ICollsionWithPlayer.hpp"

#include <Engine/Sprite.hpp>

#include <memory>

using namespace ValyrianEngine;

class TerminationFlag : public Sprite, public ICollsionWithPlayer {
public:
	static constexpr const char* s_terminFlagType = "TerminationFlag";

	TerminationFlag(int x, int y, int tilesDown);
	virtual ~TerminationFlag();

	void registerCollision() override;
	void startAnimation(uint64_t time = getSystemTime()) override;

private:
	std::shared_ptr<spdlog::logger> m_logger;
	void initializeAnimations();

	int m_tilesDown;

	std::unique_ptr<MovingAnimator> m_moveAnimator;
	std::unique_ptr<MovingAnimation> m_moveAnim;
};

#endif
