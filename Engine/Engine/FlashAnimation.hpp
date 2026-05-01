#ifndef FLASHANIMATION_HPP
#define FLASHANIMATION_HPP

#include "Animation.hpp"

namespace ValyrianEngine {

class FlashAnimation : public Animation {
public:
	FlashAnimation() = default;
	FlashAnimation(const std::string& id, unsigned du, float hide)
	    : Animation(id),
	      m_reps{du},
	      m_hideDelay{hide * 1000} {}

	void setHideDelay(float d) {
		m_hideDelay = d;
	}

	void setReps(float r) {
		m_reps = r;
	}

	float getReps() const {
		return m_reps;
	}

	float getHideDelay() const {
		return m_hideDelay;
	}

private:
	unsigned m_reps{0};
	float m_hideDelay{0};
};

} // namespace ValyrianEngine
#endif
