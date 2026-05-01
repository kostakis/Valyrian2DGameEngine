#ifndef COIN_HPP
#define COIN_HPP

#include "FordwardDeclarations.hpp"
#include "ICollsionWithPlayer.hpp"

#include <Engine/Sprite.hpp>
#include <tmxlite/Layer.hpp>

#include <memory>

namespace ValyrianEngine {

class Coin : public Sprite, public ICollsionWithPlayer {
public:
	static constexpr const char* s_type = "Coin";
	static void s_coinFactory(const tmx::Layer::Ptr& spritesLayer);

	Coin(int x, int y);
	virtual ~Coin();

	void startAnimation(uint64_t time = getSystemTime());
	void take(uint64_t time = getSystemTime());
	bool isTaken() const;

	void registerCollision() override;

private:
	Bitmap m_coinBitmap{nullptr};

	bool m_isTaken{false};

	std::unique_ptr<FrameRangeAnimator> m_frameRangeAnimator;
	std::unique_ptr<FrameRangeAnimation> m_frameRangeAnimation;
	std::unique_ptr<MovingAnimator> m_movingAnimator;
	std::unique_ptr<MovingAnimation> m_movingAnimation;

	void initAnims();
};

} // namespace ValyrianEngine

#endif
