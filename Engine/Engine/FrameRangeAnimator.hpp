#ifndef FRAMERANGEANIMATOR_HPP
#define FRAMERANGEANIMATOR_HPP

#include "Animator.hpp"
#include "FrameRangeAnimation.hpp"
#include "MyDebug.hpp"

#include <iostream>

namespace ValyrianEngine {

class FrameRangeAnimator : public Animator {
public:
	FrameRangeAnimator(void) = default;

	virtual ~FrameRangeAnimator() = default;

	void progress(timestamp_t currTime) {
		while(currTime > lastTime && (currTime - lastTime) >= anim->getDelay()) {
			if(currFrame == anim->getEndFrame()) {
				assert(anim->isForever() || currRep < anim->getReps());
				currFrame = anim->getStartFrame(); // flip to start
			} else
				++currFrame;

			lastTime += anim->getDelay();
			notifyAction(*anim);
			if(currFrame == anim->getEndFrame()) {
				if(!anim->isForever() && ++currRep == anim->getReps()) {
					state = AnimatorState::FINISHED;
					notifyStopped();
					return;
				}
			}
		}
	}

	void start(FrameRangeAnimation* a, timestamp_t t) {
		assert(a);

		anim = a;
		lastTime = t;
		state = AnimatorState::RUNNING;
		currFrame = anim->getStartFrame();
		currRep = 0;
		notifyStarted();
		notifyAction(*anim);
	}

	unsigned getCurrFrame(void) const {
		return currFrame;
	}

	unsigned getCurrRep(void) const {
		return currRep;
	}

protected:
	FrameRangeAnimation* anim = nullptr;
	unsigned currFrame = 0;
	unsigned currRep = 0;
};

} // namespace ValyrianEngine
#endif
