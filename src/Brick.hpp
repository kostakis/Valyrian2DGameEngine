#ifndef BRICK_HPP
#define BRICK_HPP

#include "FordwardDeclarations.hpp"
#include "ICollsionWithPlayer.hpp"
#include "MarioPlayer.hpp"

#include <Engine/Sprite.hpp>

#include <iostream>
#include <memory>

namespace ValyrianEngine {

class Brick : public Sprite, public ICollsionWithPlayer {
public:
	static constexpr const char* s_brickType = "Brick";

	Brick(int x, int y, Game& game, Renderer& renderer);
	virtual ~Brick();

	virtual void render(Renderer& renderer, int xOff, int yOff, const ViewData& vw) override;
	bool collisionCheck(const Sprite* s) const override;

	void registerCollision() override;

private:
	Bitmap singleBitmap{nullptr};
	Bitmap singleBitmapSmall[4]{nullptr, nullptr, nullptr, nullptr};
	MarioPlayer* m_player;
	bool destroyed{false};
	int startingX;
	int startingY;
	bool m_soundPlayed{false};

	std::unique_ptr<MovingAnimator> m_moveAnimator;
	std::unique_ptr<MovingAnimation> m_breakMovAnim;

	/*Private Methods*/
	void createBitmaps(Renderer& renderer);
	void initAnims();
};

} // namespace ValyrianEngine

#endif
