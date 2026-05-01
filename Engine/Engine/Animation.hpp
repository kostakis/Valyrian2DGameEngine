#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>

namespace ValyrianEngine {
class Animation {
public:
	Animation() = default;
	Animation(const std::string& id) {
		m_id = id;
	}
	virtual ~Animation() = default;

	void setId(const std::string& _id) {
		m_id = _id;
	}
	std::string getId() const {
		return m_id;
	}

protected:
	std::string m_id;
};

} // namespace ValyrianEngine

#endif
