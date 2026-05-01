#include "MovingAnimation.hpp"

namespace ValyrianEngine {

MovingAnimation::MovingAnimation(const std::string& id, unsigned _reps, int _dx, int _dy, unsigned _delay)
    : Animation(id),
      reps(_reps),
      dx(_dx),
      dy(_dy),
      delay(_delay) {}

MovingAnimation& MovingAnimation::setDx(int v) {
	dx = v;
	return *this;
}

MovingAnimation& MovingAnimation::setDy(int v) {
	dy = v;
	return *this;
}

MovingAnimation& MovingAnimation::setDelay(unsigned v) {
	delay = v;
	return *this;
}

MovingAnimation& MovingAnimation::setReps(unsigned n) {
	reps = n;
	return *this;
}

MovingAnimation& MovingAnimation::setForever(void) {
	reps = 0;
	return *this;
}

int MovingAnimation::getDx(void) const {
	return dx;
}

int MovingAnimation::getDy(void) const {
	return dy;
}

unsigned MovingAnimation::getDelay(void) const {
	return delay;
}

unsigned MovingAnimation::getReps(void) const {
	return reps;
}

bool MovingAnimation::isForever(void) const {
	return reps == 0;
}

} // namespace ValyrianEngine