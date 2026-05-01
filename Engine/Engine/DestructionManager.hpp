#ifndef DESTRUCTIONMANAGER_HPP
#define DESTRUCTIONMANAGER_HPP

#include "FordwardDeclarations.hpp"
#include "Logger.hpp"
#include "MyDebug.hpp"

#include <iostream>
#include <list>

namespace ValyrianEngine {

class LatelyDestroyable;
class DestructionManager {
public:
	void Register(LatelyDestroyable* d);
	void commit();
	void Clear();

	static DestructionManager& getInstance();

private:
	DestructionManager() = default;
	~DestructionManager() = default;

	std::list<LatelyDestroyable*> dead;
};

class LatelyDestroyable {
public:
	LatelyDestroyable() {
		m_logger = getOrCreateLogger("LatelyDestroyable");
		m_logger->trace("LatelyDestroyable constructor");
	}

	virtual ~LatelyDestroyable() {
		assert(m_dying);
		m_logger->trace("LatelyDestroyable destructor");
	}

	void Destroy() {
		if(m_alive) {
			m_alive = false;
			DestructionManager::getInstance().Register(this);
		}
	}

	void PrepareForManualDelete() {
		m_dying = true;
	}

	bool getAlive() const {
		return m_alive;
	}

protected:
	std::shared_ptr<spdlog::logger> m_logger;
	friend class DestructionManager;

	bool m_alive = true;
	bool m_dying = false;
	void Delete();
};

} // namespace ValyrianEngine
#endif
