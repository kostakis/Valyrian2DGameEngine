#ifndef FRAMERANGEANIMATION_HPP
#define FRAMERANGEANIMATION_HPP

#include "MovingAnimation.hpp"

#include <string>

namespace ValyrianEngine {

class FrameRangeAnimation : public MovingAnimation {
public:
	FrameRangeAnimation(void) = default;
	/**
	 * @param s First frame
	   @param e Last frame
	 */
	FrameRangeAnimation(const std::string& _id, unsigned s, unsigned e, unsigned r, int dx, int dy, int d)
	    : start(s),
	      end(e),
	      MovingAnimation(_id, r, dx, dy, d) {}

	virtual ~FrameRangeAnimation() = default;

	Animation* clone(void) const;

	FrameRangeAnimation& setStartFrame(unsigned v) {
		start = v;
		return *this;
	}

	FrameRangeAnimation& setEndFrame(unsigned v) {
		end = v;
		return *this;
	}

	unsigned getStartFrame(void) const {
		return start;
	}

	unsigned getEndFrame(void) const {
		return end;
	}

protected:
	unsigned start = 0;
	unsigned end = 0;
};

} // namespace ValyrianEngine
#endif
