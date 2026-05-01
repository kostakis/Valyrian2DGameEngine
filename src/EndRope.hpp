#ifndef ENDROPE_HPP
#define ENDROPE_HPP

#include "ICollsionWithPlayer.hpp"

#include <Engine/Sprite.hpp>

namespace ValyrianEngine {

class EndRope : public Sprite, public ICollsionWithPlayer {
public:
	EndRope(int x, int y);
	virtual ~EndRope();

	void registerCollision() override;

private:
};

} // namespace ValyrianEngine

#endif
