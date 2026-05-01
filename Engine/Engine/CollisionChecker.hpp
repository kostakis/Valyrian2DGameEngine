#ifndef COLLISIONCHECKER_HPP
#define COLLISIONCHECKER_HPP

#include "FordwardDeclarations.hpp"
#include "Sprite.hpp"

#include <functional>

namespace ValyrianEngine {

class CollisionChecker final {
private:
	using Action = std::function<void(Sprite* s1, Sprite* s2)>;
	using Entry = std::tuple<Sprite*, Sprite*, Action>;

public:
	~CollisionChecker();

	template <typename T> void Register(Sprite* s1, Sprite* s2, const T& f) {
		entries.push_back(std::make_tuple(s1, s2, f));
	}
	void Cancel(Sprite* s1, Sprite* s2);
	void Cancel(Sprite* s2);
	void Check() const;
	void Clear();

	bool collisionType(const Sprite* s1, const Sprite* s2);

	static CollisionChecker& getInstance();

private:
	CollisionChecker();
	CollisionChecker(const CollisionChecker&) = delete;
	CollisionChecker(CollisionChecker&&) = delete;

	std::list<Entry> entries;
	std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace ValyrianEngine
#endif
