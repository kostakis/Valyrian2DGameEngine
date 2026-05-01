#include "GravityHandler.hpp"

void ValyrianEngine::GravityHandler::Check(const Rectangle& r) {
	if(gravityAddicted) {
		if(onSolidGround(r)) {
			if(isFalling) {
				isFalling = false;
				onStopFalling();
			}
		} else if(!isFalling) {
			isFalling = true;
			onStartFalling();
		}
	}
}