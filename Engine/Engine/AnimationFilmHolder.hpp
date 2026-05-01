#ifndef ANIMATIONFILMHOLDER_HPP
#define ANIMATIONFILMHOLDER_HPP

#include "AnimationFilm.hpp"
#include "FordwardDeclarations.hpp"
#include "IniConfig.hpp"
#include "Rectangle.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ValyrianEngine {

class AnimationFilmHolder final {
public:
	~AnimationFilmHolder();

	void loadAll(const std::string& fileName);
	void cleanUp();

	static AnimationFilmHolder& getInstance();
	AnimationFilm* getFilm(const std::string& id);

	AnimationFilmHolder();
	AnimationFilmHolder(const AnimationFilmHolder&) = delete;
	AnimationFilmHolder(AnimationFilmHolder&&) = delete;

	std::map<std::string, AnimationFilm*> m_films;
	std::vector<IniConfig> m_configs;
	std::shared_ptr<spdlog::logger> m_logger;
};

} // namespace ValyrianEngine
#endif
