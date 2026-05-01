#include "Animator.hpp"
#include "AnimatorManager.hpp"

namespace ValyrianEngine {

Animator::Animator() {
	AnimatorManager::getInstance().registerAnimator(this);
}

Animator::~Animator() {
	if(!hasFinished()) {
		state = AnimatorState::STOPPED;
		AnimatorManager::getInstance().markAsSuspended(this);
	}
	AnimatorManager::getInstance().cancel(this);
}

bool Animator::hasFinished(void) const {
	return state != AnimatorState::RUNNING;
}

void Animator::stop(void) {
	finish(true);
}

void Animator::emergencyStop(void) {
	if(!hasFinished()) {
		state = AnimatorState::STOPPED;
		AnimatorManager::getInstance().markAsSuspended(this);
		// No onFinish callback triggered!
	}
}

void Animator::timeShift(timeStamp offset) {
	lastTime += offset;
}

bool Animator::isRunnig() const {
	return state == AnimatorState::RUNNING ? true : false;
}

void Animator::notifyStarted() {
	AnimatorManager::getInstance().markAsRunning(this);
	if(onStart) {
		onStart(this);
	}
}

void Animator::notifyAction(const Animation& anim) {
	if(onAction) {
		onAction(this, anim);
	}
}

void Animator::notifyStopped(void) {
	AnimatorManager::getInstance().markAsSuspended(this);
	if(onFinish) {
		onFinish(this);
	}
}

void Animator::finish(bool isForced) {
	if(!hasFinished()) { // If animator is in running mode
		state = isForced ? AnimatorState::STOPPED : AnimatorState::FINISHED;
		notifyStopped();
	}
}

} // namespace ValyrianEngine