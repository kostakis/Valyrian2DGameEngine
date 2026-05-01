#ifndef GRAVITYHANDLER_HPP
#define GRAVITYHANDLER_HPP

#include "Rectangle.hpp"

#include <functional>

namespace ValyrianEngine {

class GravityHandler {
public:
	using OnSolidGroundPred = std::function<bool(const Rectangle&)>;
	using OnStartFalling = std::function<void(void)>;
	using OnStopFalling = std::function<void(void)>;

	bool gravityAddicted = true;
	bool isFalling = false;
	OnSolidGroundPred onSolidGround;
	OnStartFalling onStartFalling;
	OnStopFalling onStopFalling;

public:
	template <typename T> void SetOnStartFalling(const T& f) {
		onStartFalling = f;
	}
	template <typename T> void SetOnStopFalling(const T& f) {
		onStopFalling = f;
	}
	template <typename T> void SetOnSolidGround(const T& f) {
		onSolidGround = f;
	}
	void Reset(void) {
		isFalling = false;
	}

	void Check(const Rectangle& r);
};

} // namespace ValyrianEngine
#endif
