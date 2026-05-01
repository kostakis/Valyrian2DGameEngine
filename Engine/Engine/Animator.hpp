#ifndef ANIMATOR_HPP
#define ANIMATOR_HPP

#include "Animation.hpp"
#include "Typedefs.hpp"

#include <functional>

namespace ValyrianEngine {

typedef uint64_t timestamp_t;

enum class AnimatorState { FINISHED = 0, RUNNING = 1, STOPPED = 2 };

class Animator {
public:
	Animator(const Animator&) = delete;
	Animator(Animator&&) = delete;
	using OnFinish = std::function<void(Animator*)>;
	using OnStart = std::function<void(Animator*)>;
	using OnAction = std::function<void(Animator*, const Animation&)>;

public:
	Animator();
	virtual ~Animator();

	/*Setters*/
	template <typename Tfunc> void setOnFinish(const Tfunc& f) {
		onFinish = f;
	}
	template <typename Tfunc> void setOnStart(const Tfunc& f) {
		onStart = f;
	}
	template <typename Tfunc> void setOnAction(const Tfunc& f) {
		onAction = f;
	}

	bool hasFinished(void) const;
	void stop(void);
	void emergencyStop(void);
	virtual void timeShift(timeStamp offset);
	bool isRunnig() const;

	virtual void progress(timeStamp currTime) = 0;

protected:
	timeStamp lastTime = 0;
	AnimatorState state = AnimatorState::FINISHED;

	OnStart onStart = nullptr;
	OnFinish onFinish = nullptr;
	OnAction onAction = nullptr;

	void notifyStarted();
	void notifyAction(const Animation& anim);
	void notifyStopped(void);
	void finish(bool isForced = false);
};

} // namespace ValyrianEngine
#endif
