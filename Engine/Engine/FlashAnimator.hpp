#ifndef FLASHANIMATOR_HPP
#define FLASHANIMATOR_HPP

#include "Animator.hpp"
#include "FlashAnimation.hpp"

#include <iostream>

namespace ValyrianEngine {

class FlashAnimator : public Animator {
public:
	FlashAnimator() = default;
	virtual ~FlashAnimator() = default;

	void progress(timestamp_t currTime) {
		auto hideDelay = anim->getHideDelay();

		while((currTime > lastTime) && (static_cast<float>((currTime - lastTime)) >= hideDelay)) {
			lastTime += hideDelay;
			notifyAction(*anim);
			m_currState = m_currState == false ? true : false;

			if(++m_count >= anim->getReps()) {
				state = AnimatorState::FINISHED;
				notifyStopped();
				return;
			}
		}
	}

	void start(FlashAnimation* a, timestamp_t t) {
		anim = a;
		lastTime = t;
		state = AnimatorState::RUNNING;
		m_currState = false;
		notifyStarted();
	}

	bool getState() const {
		return m_currState;
	}

	void reset() {
		m_count = 0;
	}

private:
	FlashAnimation* anim{nullptr};
	bool m_currState{0}; // 0 do hide, 1 do show
	int m_count{0};
};

} // namespace ValyrianEngine
#endif
