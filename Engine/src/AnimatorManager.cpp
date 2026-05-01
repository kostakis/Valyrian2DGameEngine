#include "AnimatorManager.hpp"
#include "MyDebug.hpp"

#include <memory>

namespace ValyrianEngine {

void AnimatorManager::registerAnimator(Animator* a) {
	assert(a->hasFinished());
	suspended.push_back(a);
}

void AnimatorManager::cancel(Animator* a) {
	assert(a->hasFinished());
	suspended.remove(a);
}

void AnimatorManager::markAsRunning(Animator* a) {
	bool returning = a->hasFinished();
	assert(!a->hasFinished());
	suspended.remove(a);
	running.push_back(a);
}

void AnimatorManager::markAsSuspended(Animator* a) {
	assert(a->hasFinished());
	running.remove(a);
	suspended.push_back(a);
}

void AnimatorManager::progress(timeStamp currTime) {
	auto copied(running);
	for(auto* a : copied) {
		a->progress(currTime);
	}
}

void AnimatorManager::timeShift(timeStamp newTime) {
	for(auto& i : running) {
		i->timeShift(newTime);
	}
}

void AnimatorManager::stopAll() {
	auto copied(running);
	for(auto* a : copied) {
		a->emergencyStop();
	}
}

AnimatorManager& AnimatorManager::getInstance() {
	static AnimatorManager instance;
	return instance;
}

} // namespace ValyrianEngine