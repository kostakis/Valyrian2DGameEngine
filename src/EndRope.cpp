#include "EndRope.hpp"
#include "MarioPlayer.hpp"

#include <Engine/CollisionChecker.hpp>
#include <Engine/SpriteManager.hpp>

namespace ValyrianEngine {

EndRope::EndRope(int x, int y)
    : Sprite(x, y, "EndRope", "Created", nullptr) {
	m_logger = getOrCreateLogger("EndRope");
	m_logger->trace("EndRope constructor");
	isVisible = false;
}

EndRope::~EndRope() {
	m_logger->trace("EndRope destructor");
}

void EndRope::registerCollision() {
	auto playerList = SpriteManager::getInstance().getTypeList(MarioPlayer::s_type);
	if(!playerList.empty()) {
		auto* player = playerList.front();
		CollisionChecker::getInstance().Register(this, player, [](Sprite* endRope, Sprite* p) {
			CollisionChecker::getInstance().Cancel(endRope);
			static_cast<MarioPlayer*>(p)->startFlagDown();
		});
	}
}

} // namespace ValyrianEngine
