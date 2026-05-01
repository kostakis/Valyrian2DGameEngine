#include "CollisionChecker.hpp"
#include "Logger.hpp"

#include <algorithm>

namespace ValyrianEngine {

CollisionChecker::CollisionChecker() {
	m_logger = getOrCreateLogger("CollisionChecker");
	m_logger->trace("CollisionChecker constructor");
}

CollisionChecker::~CollisionChecker() {
	m_logger->trace("CollisionChecker destructor");
	entries.clear();
}

void CollisionChecker::Cancel(Sprite* s1, Sprite* s2) {
	auto i = std::find_if(entries.begin(), entries.end(), [s1, s2](const Entry& e) {
		return std::get<0>(e) == s1 && std::get<1>(e) == s2;
	});
	entries.erase(i);
	m_logger->trace("Collisions list size: {}", entries.size());
}

void CollisionChecker::Cancel(Sprite* s2) {
	auto i = entries.begin();
	while((i = std::find_if(entries.begin(), entries.end(), [s2](const Entry& e) -> bool {
		       return std::get<0>(e) == s2 || std::get<1>(e) == s2;
	       })) != entries.end())
		entries.erase(i);

	m_logger->trace("Collisions list size: {}", entries.size());
}

void CollisionChecker::Clear() {
	entries.clear();
	m_logger->debug("Collision list cleared");
}

void CollisionChecker::Check() const {
	auto snapshot = entries;
	for(const auto& e : snapshot) {
		if(std::get<0>(e)->collisionCheck(std::get<1>(e))) {
			std::get<2>(e)(std::get<0>(e), std::get<1>(e));
		}
	}
}

bool CollisionChecker::collisionType(const Sprite* s1, const Sprite* s2) {
	return true; // TODO
}

CollisionChecker& CollisionChecker::getInstance() {
	static CollisionChecker instance;
	return instance;
}

} // namespace ValyrianEngine