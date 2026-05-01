#ifndef ANIMATORMANAGER_HPP
#define ANIMATORMANAGER_HPP

#include "Animator.hpp"

#include <list>

namespace ValyrianEngine {

class AnimatorManager {
public:
	~AnimatorManager(void) = default;

	void registerAnimator(Animator* a);
	void cancel(Animator* a);
	void markAsRunning(Animator* a);
	void markAsSuspended(Animator* a);
	void progress(timeStamp currTime);
	void timeShift(timeStamp newTime);
	void stopAll();

	static AnimatorManager& getInstance();

private:
	std::list<Animator*> running, suspended;

	AnimatorManager(void) = default;
	AnimatorManager(const AnimatorManager&) = delete;
	AnimatorManager(AnimatorManager&&) = delete;
};

} // namespace ValyrianEngine
#endif
