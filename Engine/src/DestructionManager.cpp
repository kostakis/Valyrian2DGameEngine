#include "DestructionManager.hpp"
#include "CollisionChecker.hpp"

#include <typeinfo>

namespace ValyrianEngine {

void DestructionManager::Register(LatelyDestroyable* d) {
	assert(!d->getAlive());
	dead.push_back(d);
}

void DestructionManager::commit() {
	for(auto* i : dead) {
		CollisionChecker::getInstance().Cancel((Sprite*)i); // Classic C cast
		i->Delete();
	}
	dead.clear();
}

void DestructionManager::Clear() {
	dead.clear();
}

DestructionManager& DestructionManager::getInstance() {
	static DestructionManager singleton;
	return singleton;
}

void LatelyDestroyable::Delete() {

	assert(!m_dying);
	m_dying = true;
	m_logger->trace("Deleting {}", typeid(*this).name());
	delete this;
}

} // namespace ValyrianEngine
