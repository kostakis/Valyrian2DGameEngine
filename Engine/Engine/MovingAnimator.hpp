#ifndef MOVINGANIMATOR_HPP
#define MOVINGANIMATOR_HPP

#include "Animator.hpp"
#include "MovingAnimation.hpp"

namespace ValyrianEngine {

class MovingAnimator : public Animator {
public:
	MovingAnimator() = default;
	virtual ~MovingAnimator() {}

	void progress(timestamp_t currTime) {
		while(currTime > lastTime && (currTime - lastTime) >= anim->getDelay()) {
			lastTime += anim->getDelay();
			notifyAction(*anim);
			auto temp_a = (++currRep == anim->getReps());
			auto temp_b = !anim->isForever();
			if(temp_a && temp_b) {
				state = AnimatorState::FINISHED;
				notifyStopped();
			}
		}
	}

	void start(MovingAnimation* a, timestamp_t t) {
		anim = a;
		lastTime = t;
		state = AnimatorState::RUNNING;
		currRep = 0;
		notifyStarted();
	}

	const MovingAnimation& getAnim() const {
		return *anim;
	}

	unsigned getCurrentRep() const {
		return currRep;
	}

protected:
	MovingAnimation* anim{nullptr};
	unsigned currRep{0};
};

} // namespace ValyrianEngine
#endif
