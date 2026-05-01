#ifndef NPCBRICK_HPP
#define NPCBRICK_HPP

#include "Game.hpp"
#include "MarioPlayer.hpp"

#include <Engine/AnimationFilmHolder.hpp>
#include <Engine/CollisionChecker.hpp>
#include <Engine/MovingAnimator.hpp>
#include <Engine/Sprite.hpp>

#include <memory>

using namespace ValyrianEngine;

class NpcBrick : public Sprite {
public:
	static constexpr const char* s_brickType = "NpcBrick";

	enum NpcType { COIN, MAGIC_MUSHROOM, ONE_UP_MUSHROOM, STARMAN };

	NpcBrick(int x, int y, NpcType type, int occur, Game& game, bool visibility = true);
	virtual ~NpcBrick();

	virtual void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) override;
	bool collisionCheck(const Sprite* s) const override;
	void setIsVisible(bool visible) override;

private:
	Bitmap m_bitmap{nullptr};
	MarioPlayer* m_player{nullptr}; // Keeping a player reference so we can searching on the list

	NpcType m_type;

	int m_numberOfOcuur{0};
	bool m_takenAll{false};

	int m_ySpeed{-3};
	int m_animationSpeed{17};
	int m_howMuchUp{-18}; // Max height before going down again

	std::unique_ptr<MovingAnimator> m_moveAnimator;
	std::unique_ptr<MovingAnimation> m_moveAnim;

	void initAnimations();
};

#endif
