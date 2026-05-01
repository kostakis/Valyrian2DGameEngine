#ifndef MOVINGANIMATION_HPP
#define MOVINGANIMATION_HPP

#include "Animation.hpp"

#include <string>

namespace ValyrianEngine {

class MovingAnimation : public Animation {
public:
	MovingAnimation() = default;
	MovingAnimation(const std::string& id, unsigned _reps, int _dx, int _dy, unsigned _delay);

	virtual ~MovingAnimation() = default;

	MovingAnimation& setDx(int v);
	MovingAnimation& setDy(int v);
	MovingAnimation& setDelay(unsigned v);
	MovingAnimation& setReps(unsigned n);
	MovingAnimation& setForever();

	int getDx() const;
	int getDy() const;
	unsigned getDelay() const;
	unsigned getReps() const;
	bool isForever() const;

private:
	unsigned reps = 1;  // 0=forever
	int dx = 0, dy = 0; // How much the object will move
	unsigned delay = 0; // Delay for the movement
};

} // namespace ValyrianEngine
#endif
